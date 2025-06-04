import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

def plot_stat_histograms(csv_file='pokemon_data.csv'):
    """
    Reads Pokémon data, filters out special Pokémon and forms,
    and generates histograms for each of the six base stats.

    Args:
        csv_file (str): The path to the Pokémon CSV file.
    """
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

    # Convert stat columns to numeric, handling errors
    stats_columns = ['HP', 'Attack', 'Defense', 'SpecialAttack', 'SpecialDefense', 'Speed']
    for col in stats_columns:
        df[col] = pd.to_numeric(df[col], errors='coerce')
    df.dropna(subset=stats_columns, inplace=True) # Drop rows if any key stat is NaN

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

    if df_filtered.empty:
        print("No data available after filtering to plot histograms.")
        return

    # --- Generate Histograms for Each Stat ---
    print(f"Generating histograms for {len(df_filtered)} non-special Pokémon...")

    for stat in stats_columns:
        plt.figure(figsize=(8, 6)) # Create a new figure for each histogram
        
        # Plot the histogram
        # 'bins' can be adjusted for more or fewer bars. 'auto' tries to find an optimal number.
        # 'edgecolor' makes bars distinct.
        plt.hist(df_filtered[stat], bins='auto', edgecolor='black', color='skyblue', alpha=0.7)
        
        plt.xlabel(f'{stat} Stat Value', fontsize=12)
        plt.ylabel('Number of Pokémon', fontsize=12)
        plt.title(f'Distribution of {stat} Stat Values (Non-Special Pokémon)', fontsize=14)
        plt.grid(axis='y', linestyle='--', alpha=0.7)
        plt.tight_layout()
        plt.show()

# --- Run the script ---
if __name__ == "__main__":
    plot_stat_histograms('pokedex.csv') # REMEMBER TO RENAME THIS TO YOUR ACTUAL CSV FILE NAME!