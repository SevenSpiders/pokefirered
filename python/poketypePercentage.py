import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from collections import defaultdict

def plot_type_distribution_by_generation_dual_type(csv_file='pokemon_data.csv'):
    """
    Reads Pokémon data, filters out specific entries, assigns generations,
    calculates the percentage of each Pokémon type (including Type I and Type II)
    within its generation, and plots these percentages across generations.

    Args:
        csv_file (str): The path to the Pokémon CSV file.
    """
    # --- Define Pokémon Type Colors ---
    # These colors are commonly associated with Pokémon types and are chosen for visibility.
    pokemon_type_colors = {
        'Normal': '#A8A878', 'Fire': '#F08030', 'Water': '#6890F0',
        'Grass': '#78C850', 'Electric': '#F8D030', 'Ice': '#98D8D8',
        'Fighting': '#C03028', 'Poison': '#A040A0', 'Ground': '#E0C068',
        'Flying': '#A890F0', 'Psychic': '#F85888', 'Bug': '#A8B820',
        'Rock': '#B8A038', 'Ghost': '#705898', 'Dragon': '#7038F8',
        'Steel': '#B8B8D0', 'Dark': '#705848', 'Fairy': '#EE99AC',
        # Add any other types if they appear in your data and you want specific colors
    }
    default_color = '#808080' # Grey for types not in our color map

    try:
        df = pd.read_csv(csv_file)
    except FileNotFoundError:
        print(f"Error: '{csv_file}' not found. Make sure the file is in the same directory as the script.")
        return
    except Exception as e:
        print(f"Error reading CSV: {e}")
        return

    # --- Data Preprocessing ---
    df.columns = df.columns.str.strip() # Clean column names

    # Convert 'NationalDex' to numeric base ID for generation assignment
    df['NationalDex_Float'] = pd.to_numeric(df['NationalDex'], errors='coerce')
    df['NationalDex_BaseID'] = df['NationalDex_Float'].apply(np.floor).astype('Int64')
    df.dropna(subset=['NationalDex_BaseID'], inplace=True)

    # Ensure 'Type I' and 'Type II' columns are string type and clean them
    df['Type I'] = df['Type I'].astype(str).str.strip()
    df['Type II'] = df['Type II'].astype(str).str.strip()

    # Filter out entries where 'Type I' is 'nan' (for empty cells or non-existent types)
    df = df[df['Type I'].str.lower() != 'nan']

    # 1. Exclude "Mega Evolution" entries from the 'Type' column
    df = df[~df['Type'].astype(str).str.contains('Mega Evolution', case=False, na=False)]

    # 2. Exclude "Forms" like "Alolan Form" or "Galarian Form"
    df = df[~df['PokemonName'].astype(str).str.contains(r'\(|Form|Regional|Hisuian|Paldean|Galar', regex=True, case=False, na=False)]

    # 3. Exclude Legendary, Sub-Legendary, and Mythical Pokémon from the 'Type' column
    legendary_mythical_types = ['Legendary', 'Sub-Legendary', 'Mythical']
    df['IsSpecialPokemon'] = df['Type'].astype(str).str.contains(
        '|'.join(legendary_mythical_types), case=False, na=False
    )
    df_filtered = df[~df['IsSpecialPokemon']].copy()

    # --- Assign Generations ---
    generation_boundaries = {
        1: (1, 151), 2: (152, 251), 3: (252, 386),
        4: (387, 493), 5: (494, 649), 6: (650, 721),
        7: (722, 809), 8: (810, 905), 9: (906, 1025)
    }

    def get_generation(pokemon_base_id):
        for gen, (min_id, max_id) in generation_boundaries.items():
            if min_id <= pokemon_base_id <= max_id:
                return gen
        return None

    df_filtered['Generation'] = df_filtered['NationalDex_BaseID'].apply(get_generation)
    df_filtered.dropna(subset=['Generation'], inplace=True)
    df_filtered['Generation'] = df_filtered['Generation'].astype(int)

    # --- Calculate Percentage of each Type (Type I & Type II) per Generation ---

    type_counts_per_gen = defaultdict(lambda: defaultdict(int))
    total_pokemon_per_gen = defaultdict(int)

    for index, row in df_filtered.iterrows():
        gen = row['Generation']
        type_i = row['Type I']
        type_ii = row['Type II'] # This can be 'nan' for single-type Pokémon

        total_pokemon_per_gen[gen] += 1
        type_counts_per_gen[gen][type_i] += 1

        # If Type II exists and is not 'nan' (or equivalent for missing data)
        if type_ii.lower() != 'nan' and type_ii != '':
            type_counts_per_gen[gen][type_ii] += 1

    # Convert counts to a DataFrame for easier percentage calculation
    # First, flatten the defaultdicts
    data_for_df = []
    for gen, types_count in type_counts_per_gen.items():
        for pokemon_type, count in types_count.items():
            data_for_df.append({'Generation': gen, 'Type': pokemon_type, 'Count': count})

    counts_df = pd.DataFrame(data_for_df)

    if counts_df.empty:
        print("No type data to process after filtering and counting.")
        return

    # Pivot to get generations as index, types as columns
    type_counts_pivot = counts_df.pivot_table(index='Generation', columns='Type', values='Count', fill_value=0)

    # Convert total_pokemon_per_gen to a Series for division
    total_pokemon_series = pd.Series(total_pokemon_per_gen)

    # Calculate percentages
    # Ensure alignment of series and dataframe for division
    type_percentages_per_gen = type_counts_pivot.div(total_pokemon_series, axis=0) * 100
    type_percentages_per_gen.fillna(0, inplace=True) # Fill any potential NaNs (e.g., if a type wasn't present in a generation)

    # --- Plotting ---
    if type_percentages_per_gen.empty:
        print("No data available after calculating percentages to plot.")
        return

    plt.figure(figsize=(15, 9))

    for pokemon_type in type_percentages_per_gen.columns:
        line_color = pokemon_type_colors.get(pokemon_type, default_color)
        plt.plot(type_percentages_per_gen.index, type_percentages_per_gen[pokemon_type],
                 marker='o', label=pokemon_type, color=line_color, linewidth=2) # Added linewidth for better visibility

    plt.xlabel('Generation', fontsize=12)
    plt.ylabel('Percentage of Pokémon (%)', fontsize=12)
    plt.title('Distribution of Primary & Secondary Pokémon Types Across Generations (Excluding Forms & Special Pokémon)', fontsize=14)
    plt.xticks(type_percentages_per_gen.index)
    plt.ylim(bottom=0)
    plt.grid(True, linestyle='--', alpha=0.7)
    # Adjust legend position and spacing
    plt.legend(title='Pokémon Type', bbox_to_anchor=(1.02, 1), loc='upper left', borderaxespad=0.)
    plt.tight_layout(rect=[0, 0, 0.88, 1])

    plt.show()

# --- Run the script ---
if __name__ == "__main__":
    plot_type_distribution_by_generation_dual_type('pokedex.csv') # Make sure this matches your CSV file name!