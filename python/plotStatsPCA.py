import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn.decomposition import PCA
from sklearn.preprocessing import StandardScaler
import os # Import os to check for file existence

def plot_pokemon_stats_PCA(csv_file='pokedex.csv'):
    """
    Reads Pokémon data from a CSV, normalizes individual stats by dividing by 'Total' stat,
    performs Principal Component Analysis (PCA) on these normalized stats (reducing to 1 dimension),
    and plots this principal component against their original 'Total' stats.
    Each point represents a Pokémon, labeled by its NationalDex number.

    Args:
        csv_file (str): The path to the Pokémon CSV file.
    """
    if not os.path.exists(csv_file):
        print(f"Error: '{csv_file}' not found. Please ensure the file is in the same directory or provide the correct path.")
        return

    try:
        df = pd.read_csv(csv_file)
    except Exception as e:
        print(f"Error reading CSV: {e}")
        return

    # Define the columns containing the Pokémon stats for PCA
    stat_columns = ['HP', 'Attack', 'Defense', 'SpecialAttack', 'SpecialDefense', 'Speed']
    national_dex_column = 'NationalDex'
    total_stats_column = 'Total' # New column for x-axis

    # Check if all required stat columns exist in the DataFrame for PCA
    if not all(col in df.columns for col in stat_columns):
        missing_cols = [col for col in stat_columns if col not in df.columns]
        print(f"Error: Missing required stat columns for PCA in the CSV: {missing_cols}")
        print(f"Available columns: {df.columns.tolist()}")
        return

    # Check if the NationalDex column exists for labeling
    if national_dex_column not in df.columns:
        print(f"Error: Required column '{national_dex_column}' not found in the CSV. Cannot label points.")
        print(f"Available columns: {df.columns.tolist()}")
        return

    # Check if the 'Total' column exists for the x-axis and for normalization
    if total_stats_column not in df.columns:
        print(f"Error: Required column '{total_stats_column}' not found in the CSV. Cannot plot 'Total' stats or normalize by it.")
        print(f"Available columns: {df.columns.tolist()}")
        return

    # Extract the stats data for PCA
    pokemon_stats_for_pca = df[stat_columns].copy() # Use .copy() to avoid SettingWithCopyWarning
    original_total_stats = df[total_stats_column]

    # --- Actual math here ---

    # 0. Normalize individual stats by dividing by the 'Total' stat
    # Ensure 'Total' is not zero to prevent division by zero errors.
    # Replace 0s with a small epsilon or handle appropriately if your data can have 0 Total.
    # Here, we assume 'Total' stats are always positive.
    if (original_total_stats == 0).any():
        print("Warning: Some Pokémon have a 'Total' stat of 0, which will cause division by zero. Replacing 0 with 1 for normalization.")
        total_stats_divisor = original_total_stats.replace(0, 1) # Avoid division by zero
    else:
        total_stats_divisor = original_total_stats

    # Apply element-wise division for normalization
    normalized_pokemon_stats = pokemon_stats_for_pca.div(total_stats_divisor, axis=0)


    # 1. Standardize the *normalized* data for PCA
    scaler = StandardScaler()
    scaled_normalized_stats = scaler.fit_transform(normalized_pokemon_stats)

    # 2. Perform PCA with n_components=1 for the y-axis
    pca = PCA(n_components=1)
    principal_component_1d = pca.fit_transform(scaled_normalized_stats)

    # Flatten the 1D principal component array for easier plotting
    principal_component_1d = principal_component_1d.flatten()

    # Create a DataFrame for plotting, including Original Total stats and NationalDex
    plot_df = pd.DataFrame({
        'Total_Stats': original_total_stats, # Use the original Total for plotting X-axis
        'Principal_Component': principal_component_1d,
        'NationalDex': df[national_dex_column]
    })

    # 3. Plot the results
    plt.figure(figsize=(14, 10)) # Increased figure size for better label visibility and wider spread

    # Plot the scatter points
    plt.scatter(plot_df['Total_Stats'],
                plot_df['Principal_Component'],
                alpha=0.7, # Add some transparency
                s=50,      # Size of the points
                color='red', # Color of the points
                label='Pokémon') # Label for the legend (optional)

    # Annotate each point with its NationalDex
    for i, row in plot_df.iterrows():
        plt.annotate(str(row['NationalDex']),
                     (row['Total_Stats'], row['Principal_Component']),
                     textcoords="offset points",
                     xytext=(0,10), # Offset for the label from the point (adjusted to be above the point)
                     ha='center', # Horizontal alignment
                     fontsize=8, # Smaller font size to avoid too much overlap
                     color='blue') # Color for the labels

    plt.xlabel('Original Total Stats')
    plt.ylabel(f'Principal Component 1 (of Normalized Stats, {pca.explained_variance_ratio_[0]*100:.2f}% variance)')
    plt.title('PCA of Normalized Pokémon Base Stats (1D) vs. Original Total Stats')
    plt.grid(True)
    plt.axhline(0, color='gray', linestyle='--', linewidth=0.8) # Horizontal line for PC 0

    plt.legend() # Show the legend for the scatter plot
    plt.tight_layout()
    plt.show()

    print("\nExplained Variance Ratio for the 1st principal component (after normalization by Total):")
    print(f"Principal Component 1: {pca.explained_variance_ratio_[0]*100:.2f}%")

    # Print the PCA vector (components)
    print("\nPrincipal Component (PCA Vector) for the normalized and scaled stats:")
    # The components_ attribute will be a 2D array, even for n_components=1.
    # We take the first (and only) row.
    print(f"Features: {stat_columns}")
    print(f"Vector: {pca.components_[0]}")


# --- Run the script ---
if __name__ == "__main__":
    plot_pokemon_stats_PCA('pokedex.csv')
