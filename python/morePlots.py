import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import matplotlib.colors as mcolors

def plot_pokemon_insights(csv_file='pokemon_data.csv'):
    """
    Reads Pokémon data, filters it, and generates multiple plots:
    1. Average of each individual base stat per generation.
    2. Distribution of Evolution Methods.
    3. Average Total Stats for Single-Type vs. Dual-Type Pokémon.
    """
    try:
        df = pd.read_csv(csv_file)
    except FileNotFoundError:
        print(f"Error: '{csv_file}' not found. Make sure the file is in the same directory as the script.")
        return
    except Exception as e:
        print(f"Error reading CSV: {e}")
        return

    # --- Data Preprocessing (Re-used from previous scripts) ---
    df.columns = df.columns.str.strip() # Clean column names

    # Convert 'NationalDex' to numeric base ID for generation assignment
    df['NationalDex_Float'] = pd.to_numeric(df['NationalDex'], errors='coerce')
    df['NationalDex_BaseID'] = df['NationalDex_Float'].apply(np.floor).astype('Int64')
    df.dropna(subset=['NationalDex_BaseID'], inplace=True)

    # Convert 'Total' and individual base stats to numeric, handling errors
    stats_columns = ['HP', 'Attack', 'Defense', 'SpecialAttack', 'SpecialDefense', 'Speed', 'Total']
    for col in stats_columns:
        df[col] = pd.to_numeric(df[col], errors='coerce')
    df.dropna(subset=stats_columns, inplace=True) # Drop rows if any key stat is NaN

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

    # --- Assign Generations (Re-used) ---
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

    # --- Plot 1: Average of Each Individual Base Stat per Generation ---
    plt.figure(figsize=(12, 7))
    
    # Select only the relevant stat columns and the 'Generation' column
    stats_by_gen = df_filtered.groupby('Generation')[['HP', 'Attack', 'Defense', 'SpecialAttack', 'SpecialDefense', 'Speed']].mean()

    # Define some distinct colors for the stats
    stat_colors = {
        'HP': 'red', 'Attack': 'orange', 'Defense': 'forestgreen',
        'SpecialAttack': 'blue', 'SpecialDefense': 'purple', 'Speed': 'cyan'
    }

    for stat in stats_by_gen.columns:
        plt.plot(stats_by_gen.index, stats_by_gen[stat], marker='o', label=stat, color=stat_colors.get(stat, 'gray'))

    plt.xlabel('Generation', fontsize=12)
    plt.ylabel('Average Stat Value', fontsize=12)
    plt.title('Average Individual Base Stats per Generation (Excluding Forms & Special Pokémon)', fontsize=14)
    plt.xticks(stats_by_gen.index) # Ensure ticks for each generation
    plt.ylim(bottom=0)
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend(title='Base Stat', bbox_to_anchor=(1.05, 1), loc='upper left')
    plt.tight_layout()
    plt.show()

    # --- Plot 2: Distribution of Evolution Methods ---
    plt.figure(figsize=(10, 6))

    # Clean the 'EvolutionMethod' column
    # Ensure it's string, strip whitespace, and treat empty/NaN as 'No Evolution'
    df_filtered['EvolutionMethod_Cleaned'] = df_filtered['EvolutionMethod'].astype(str).str.strip()
    df_filtered['EvolutionMethod_Cleaned'] = df_filtered['EvolutionMethod_Cleaned'].replace('nan', 'No Evolution', regex=True)
    df_filtered['EvolutionMethod_Cleaned'] = df_filtered['EvolutionMethod_Cleaned'].replace('', 'No Evolution', regex=False)


    # Extract just the method, ignoring details like "Lv. 16" -> "Lv." or "Item: Fire Stone" -> "Item"
    # This requires a bit of regex or string manipulation.
    # For simplicity, let's categorize based on common patterns.
    def categorize_evolution_method(method_str):
        method_str = method_str.lower()
        if 'no evolution' in method_str:
            return 'No Evolution'
        elif 'lv.' in method_str or 'level' in method_str:
            return 'Level Up'
        elif 'item' in method_str:
            return 'Item'
        elif 'trade' in method_str:
            return 'Trade'
        elif 'friendship' in method_str or 'happiness' in method_str:
            return 'Friendship'
        elif 'stone' in method_str: # Catch specific stones if not caught by 'Item'
            return 'Item'
        elif 'day' in method_str or 'night' in method_str or 'time' in method_str:
            return 'Time-based'
        elif 'location' in method_str:
            return 'Location'
        elif 'gender' in method_str:
            return 'Gender-specific'
        elif 'move' in method_str or 'knows move' in method_str:
            return 'Knowing a Move'
        elif 'ability' in method_str:
            return 'Ability-related'
        elif 'stats' in method_str:
            return 'Stat-related'
        elif 'party' in method_str:
            return 'Party-related'
        elif 'courage' in method_str or 'strong style' in method_str or 'agile style' in method_str: # PLA specific
            return 'Style/Condition'
        else:
            return 'Other/Complex' # Catch all for less common or unparsed methods

    df_filtered['EvolutionMethod_Category'] = df_filtered['EvolutionMethod_Cleaned'].apply(categorize_evolution_method)

    evolution_method_counts = df_filtered['EvolutionMethod_Category'].value_counts()

    if evolution_method_counts.empty:
        print("No evolution method data to plot after filtering.")
        return

    # Use a color map for bars
    colors = plt.cm.Paired(np.linspace(0, 1, len(evolution_method_counts)))

    bars = plt.bar(evolution_method_counts.index, evolution_method_counts.values, color=colors, edgecolor='black')

    plt.xlabel('Evolution Method Category', fontsize=12)
    plt.ylabel('Number of Pokémon', fontsize=12)
    plt.title('Distribution of Pokémon Evolution Methods (Excluding Forms & Special Pokémon)', fontsize=14)
    plt.xticks(rotation=45, ha='right')
    plt.grid(axis='y', linestyle='--', alpha=0.7)
    plt.tight_layout()

    # Add counts on top of bars
    for bar in bars:
        yval = bar.get_height()
        plt.text(bar.get_x() + bar.get_width()/2, yval + 1, int(yval), ha='center', va='bottom', fontsize=9)

    plt.show()

    # --- Plot 3: Average Total Stats for Single-Type vs. Dual-Type Pokémon ---
    plt.figure(figsize=(8, 6))

    # Determine if a Pokémon is single or dual type
    df_filtered['IsDualType'] = df_filtered['Type II'].str.lower().ne('nan') & (df_filtered['Type II'] != '')

    single_type_avg = df_filtered[~df_filtered['IsDualType']]['Total'].mean()
    dual_type_avg = df_filtered[df_filtered['IsDualType']]['Total'].mean()

    # Handle cases where a group might be empty
    single_type_avg = single_type_avg if not pd.isna(single_type_avg) else 0
    dual_type_avg = dual_type_avg if not pd.isna(dual_type_avg) else 0

    labels = ['Single-Type Pokémon', 'Dual-Type Pokémon']
    averages = [single_type_avg, dual_type_avg]

    if not any(averages): # Check if both averages are zero/empty
        print("No data available to compare single vs dual type average stats.")
        return

    bars = plt.bar(labels, averages, color=['lightblue', 'darkblue'], edgecolor='black')

    plt.ylabel('Average Total Base Stats', fontsize=12)
    plt.title('Average Total Base Stats: Single-Type vs. Dual-Type Pokémon (Excluding Forms & Special Pokémon)', fontsize=12)
    plt.ylim(bottom=0)
    plt.grid(axis='y', linestyle='--', alpha=0.7)
    plt.tight_layout()

    # Add values on top of bars
    for bar in bars:
        yval = bar.get_height()
        plt.text(bar.get_x() + bar.get_width()/2, yval + 5, round(yval, 2), ha='center', va='bottom', fontsize=10)

    plt.show()


# --- Run the script ---
if __name__ == "__main__":
    plot_pokemon_insights('pokedex.csv') # REMEMBER TO RENAME THIS TO YOUR ACTUAL CSV FILE NAME!