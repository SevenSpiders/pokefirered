import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

def plot_pokemon_stat_groups(csv_file='pokemon_data.csv'):
    """
    Reads Pokémon data from a CSV, categorizes Pokémon into groups
    based on type and evolution status, and plots their average total stats.

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

    # 1. Standardize column names (remove leading/trailing spaces from headers)
    df.columns = df.columns.str.strip()

    # 2. Filter out "Mega Evolution" entries
    # Assuming 'Type' column contains "Mega Evolution" for these entries.
    # We'll use case-insensitive matching for robustness.
    df = df[~df['Type'].str.contains('Mega Evolution', case=False, na=False)]

    # 3. Include "Form" entries: The current filtering logic already includes them
    # as long as their 'Type' isn't "Mega Evolution".
    # Example: "Alolan Form" or "Galarian Form" in 'PokemonName' will be kept.

    # 4. Convert 'Total' column to numeric, handling potential errors
    df['Total'] = pd.to_numeric(df['Total'], errors='coerce')
    df.dropna(subset=['Total'], inplace=True) # Drop rows where Total couldn't be converted

    # --- Categorize Pokémon into Groups ---

    # Group 1: Legendary, Sub-Legendary, Mythical
    # Assuming 'Type' column contains these labels.
    # Using 'isin' for cleaner checking of multiple values.
    # Convert 'Type' column to string to handle potential non-string values gracefully.
    legendary_mythical_types = ['Legendary', 'Sub-Legendary', 'Mythical']
    df['IsLegendaryMythical'] = df['Type'].astype(str).str.contains(
        '|'.join(legendary_mythical_types), case=False, na=False
    )
    group_legendary_mythical = df[df['IsLegendaryMythical']].copy()


    # Group 2 & 3: Exclude Group 1, then check evolution method
    non_legendary_mythical_df = df[~df['IsLegendaryMythical']].copy()

    # Determine if a Pokémon has an evolution method.
    # Assuming 'EvolutionMethod' is empty/NaN for final stage forms,
    # or contains text if there's an evolution.
    # Convert 'EvolutionMethod' to string and check if it's not empty/NaN after stripping whitespace.
    non_legendary_mythical_df['HasEvolutionMethod'] = non_legendary_mythical_df['EvolutionMethod'].astype(str).str.strip().ne('')
    non_legendary_mythical_df['HasEvolutionMethod'] = non_legendary_mythical_df['HasEvolutionMethod'] & (non_legendary_mythical_df['EvolutionMethod'].astype(str).str.strip().str.lower() != 'nan')

    # Group 2: Pokémon with no evolution methods (final stage forms), excluding Group 1
    group_final_stage = non_legendary_mythical_df[~non_legendary_mythical_df['HasEvolutionMethod']].copy()

    # Group 3: Pokémon with evolution methods, excluding Group 1
    group_evolves = non_legendary_mythical_df[non_legendary_mythical_df['HasEvolutionMethod']].copy()

    # --- Calculate Average Total Stats for each group ---
    avg_legendary_mythical = group_legendary_mythical['Total'].mean()
    avg_final_stage = group_final_stage['Total'].mean()
    avg_evolves = group_evolves['Total'].mean()

    # Handle cases where a group might be empty (e.g., no legendaries found)
    # np.nanmean will return NaN if the array is empty, which matplotlib handles
    # but we'll ensure they are 0 for a cleaner plot if no data.
    avg_legendary_mythical = avg_legendary_mythical if not pd.isna(avg_legendary_mythical) else 0
    avg_final_stage = avg_final_stage if not pd.isna(avg_final_stage) else 0
    avg_evolves = avg_evolves if not pd.isna(avg_evolves) else 0

    # --- Plotting ---
    group_names = [
        "Legendary/Mythical/Sub-Legendary",
        "Final Stage (Non-Legendary)",
        "Evolves (Non-Legendary)"
    ]
    average_stats = [
        avg_legendary_mythical,
        avg_final_stage,
        avg_evolves
    ]

    plt.figure(figsize=(10, 6))
    bars = plt.bar(group_names, average_stats, color=['skyblue', 'lightgreen', 'salmon'], edgecolor='black')

    plt.ylabel('Average Total Base Stats', fontsize=12)
    plt.title('Average Total Base Stats by Pokémon Group', fontsize=14)
    plt.xticks(rotation=15, ha='right', fontsize=10) # Rotate labels slightly for readability
    plt.ylim(bottom=0) # Ensure y-axis starts at 0
    plt.grid(axis='y', linestyle='--', alpha=0.7)

    # Add the exact average value on top of each bar
    for bar in bars:
        yval = bar.get_height()
        plt.text(bar.get_x() + bar.get_width()/2, yval + 5, round(yval, 2), ha='center', va='bottom', fontsize=10)

    plt.tight_layout()
    plt.show()

# --- Run the script ---
if __name__ == "__main__":
    plot_pokemon_stat_groups('pokedex.csv') # REMEMBER TO RENAME THIS TO YOUR ACTUAL CSV FILE NAME!