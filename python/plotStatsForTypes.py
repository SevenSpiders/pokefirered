import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

def plot_average_stats_by_type_i(csv_file='pokemon_data.csv'):
    """
    Reads Pokémon data, filters out forms, legendaries, sub-legendaries, and mythicals,
    then plots the average total stats for each Pokémon Type I.

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

    # Convert 'Total' column to numeric, handling potential errors
    df['Total'] = pd.to_numeric(df['Total'], errors='coerce')
    df.dropna(subset=['Total'], inplace=True) # Drop rows where Total couldn't be converted

    # Ensure 'Type I' column is string type and clean it
    df['Type I'] = df['Type I'].astype(str).str.strip()
    # Filter out entries where 'Type I' is 'nan' (for empty cells)
    df = df[df['Type I'].str.lower() != 'nan']


    # 1. Exclude "Mega Evolution" entries
    # The 'Type' column contains "Mega Evolution" for these.
    df = df[~df['Type'].astype(str).str.contains('Mega Evolution', case=False, na=False)]

    # 2. Exclude "Forms" like "Alolan Form" or "Galarian Form"
    # This is a bit tricky as "Form" could be in 'PokemonName' or 'Type'.
    # Given the previous context, "Forms" often append to the name, e.g., "Pikachu (Alolan Form)".
    # We'll assume any 'PokemonName' containing '(' indicates a specific form we want to exclude.
    # This is a strong heuristic and might exclude desired entries if parentheses are used otherwise.
    # A more robust solution might require a separate 'IsForm' column in your data.
    df = df[~df['PokemonName'].astype(str).str.contains(r'\(', regex=True, na=False)]
    # Also exclude entries with "Form" or "Regional" in the name, case-insensitive, if not caught by parentheses
    df = df[~df['PokemonName'].astype(str).str.contains(r'Form|Regional', case=False, regex=True, na=False)]
    # You might need to adjust this filter if it's too aggressive or not aggressive enough.

    # 3. Exclude Legendary, Sub-Legendary, and Mythical Pokémon
    legendary_mythical_types = ['Legendary', 'Sub-Legendary', 'Mythical']
    df['IsLegendaryMythical'] = df['Type'].astype(str).str.contains(
        '|'.join(legendary_mythical_types), case=False, na=False
    )
    df_filtered = df[~df['IsLegendaryMythical']].copy()


    # --- Calculate Average Total Stats per Type I ---
    # Group by 'Type I' and calculate the mean of 'Total'
    average_stats_by_type = df_filtered.groupby('Type I')['Total'].mean().sort_values(ascending=False)

    # --- Plotting ---
    if average_stats_by_type.empty:
        print("No data available after filtering to plot average stats by Type I.")
        return

    plt.figure(figsize=(14, 8)) # Adjust figure size for more types
    bars = plt.bar(
        average_stats_by_type.index,      # X-axis: Pokémon Type I names
        average_stats_by_type.values,     # Y-axis: Average Total Stats
        color='skyblue',
        edgecolor='black'
    )

    plt.xlabel('Pokémon Type', fontsize=12)
    plt.ylabel('Average Total Base Stats', fontsize=12)
    plt.title('Average Total Base Stats by Primary Pokémon Type (Excluding Forms & Special Pokémon)', fontsize=14)
    plt.xticks(rotation=45, ha='right', fontsize=10) # Rotate labels for readability
    plt.ylim(bottom=0) # Ensure y-axis starts at 0
    plt.grid(axis='y', linestyle='--', alpha=0.7)

    # Add the exact average value on top of each bar
    for bar in bars:
        yval = bar.get_height()
        plt.text(bar.get_x() + bar.get_width()/2, yval + (yval * 0.01), round(yval, 2), ha='center', va='bottom', fontsize=9)

    plt.tight_layout() # Adjust layout to prevent labels from overlapping
    plt.show()

# --- Run the script ---
if __name__ == "__main__":
    plot_average_stats_by_type_i('pokedex.csv') # REMEMBER TO RENAME THIS TO YOUR ACTUAL CSV FILE NAME!