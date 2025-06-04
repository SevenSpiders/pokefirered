import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn.decomposition import PCA
from sklearn.preprocessing import StandardScaler

# --- 1. Define the Custom Types ---
custom_types = [
    "Normal", "Dark", "Psychic", "Water", "Fire", "Grass",
    "Frost", "Ground", "Steel", "Wind", "Electric"
]

# --- 2. Custom Type Matchup Table (Attacker Rows vs. Defender Columns) ---
#    Values are standard damage multipliers (1.0x, 2.0x, 0.5x, 0.0x)
#    - Mirror matchups (e.g., Normal on Normal) are 0.5x (Not Very Effective).
#    - All other interactions are currently 1.0x (Neutral) as placeholders.
#    - **EDIT THE '1.0' PLACEHOLDERS BELOW WITH YOUR CUSTOM VALUES (2.0, 0.5, 0.0)!**

raw_matchup_data = np.array([
#    NOR DAR PSY WAT FIR GRA FRO GRO STE WIN ELE  <- DEFENDING TYPES
    [1.0,1.5,0.5,1.0,1.0,1.0,1.0,1.0,0.5,1.0,1.0], # Normal (Attacking)
    [0.5,0.5,1.5,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0], # Dark
    [1.5,0.5,0.5,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0], # Psychic
    [1.0,1.0,1.0,0.5,1.5,0.5,0.5,1.0,1.0,1.0,0.5], # Water
    [1.0,1.0,1.0,0.5,0.5,1.5,1.5,0.5,1.5,0.5,1.0], # Fire
    [1.0,1.0,1.0,1.5,0.5,0.5,0.5,1.5,0.5,1.0,1.0], # Grass
    [1.0,1.0,1.0,1.5,0.5,1.5,0.5,1.0,1.0,1.0,1.0], # Frost
    [1.0,1.0,1.0,0.5,1.5,0.5,1.0,0.5,1.0,1.0,1.5], # Ground
    [1.5,1.0,1.0,1.0,0.5,1.5,1.0,1.0,0.5,1.5,0.5], # Steel
    [1.0,1.0,1.0,1.0,1.5,1.0,1.0,1.0,0.5,0.5,1.0], # Wind
    [1.0,1.0,1.0,1.5,1.0,1.0,1.0,0.5,1.5,1.0,0.5], # Electric
])

# Create a DataFrame for better readability and indexing
custom_matchup_df = pd.DataFrame(raw_matchup_data, index=custom_types, columns=custom_types)

print("Custom Type Matchup Table (Attacker rows vs. Defender columns) - Placeholder:\n")
print(custom_matchup_df)
print("\n" + "="*80 + "\n")

# --- 3. Transform the Data for PCA ---
# Neutral matchups (1.0x) become 0
# All other non-neutral matchups (2.0x, 0.5x, 0.0x) become 1
# This is crucial for PCA to analyze the "presence of a special interaction"
transformed_data = custom_matchup_df.applymap(lambda x: 0 if x == 1.0 else 1)

print("Transformed Binary Type Interaction Table (0=Neutral, 1=Non-neutral):\n")
print(transformed_data)
print("\n" + "="*80 + "\n")

# --- 4. Standardize the data for PCA ---
# Each row (Attacking Type) is a sample in 11 dimensions
X = transformed_data.values
scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)

# --- 5. Perform PCA to reduce to 2 components ---
pca = PCA(n_components=2)
principal_components = pca.fit_transform(X_scaled)

# Create a DataFrame for the PCA results
pca_df = pd.DataFrame(data=principal_components,
                      columns=['Principal Component 1', 'Principal Component 2'],
                      index=custom_types)

print("PCA Results (First 2 Principal Components):\n")
print(pca_df)
print(f"\nExplained variance by PC1: {pca.explained_variance_ratio_[0]:.2f}")
print(f"Explained variance by PC2: {pca.explained_variance_ratio_[1]:.2f}")
print(f"Total explained variance: {pca.explained_variance_ratio_.sum():.2f}")
print("\n" + "="*80 + "\n")

# --- 6. Visualize the Types in 2D PCA Space ---
plt.figure(figsize=(10, 8))
plt.scatter(pca_df['Principal Component 1'], pca_df['Principal Component 2'])

# Annotate each point with its type name
for i, type_name in enumerate(custom_types):
    plt.annotate(type_name,
                 (pca_df['Principal Component 1'].iloc[i],
                  pca_df['Principal Component 2'].iloc[i]),
                 textcoords="offset points",
                 xytext=(5,5),
                 ha='center',
                 fontsize=10,
                 bbox=dict(boxstyle="round,pad=0.3", fc="lightblue", ec="black", lw=0.5, alpha=0.6))

plt.xlabel(f'Principal Component 1 ({pca.explained_variance_ratio_[0]*100:.1f}% Variance)')
plt.ylabel(f'Principal Component 2 ({pca.explained_variance_ratio_[1]*100:.1f}% Variance)')
plt.title('Custom Pokémon Type Matchups in 2D PCA Space')
plt.grid(True)
plt.axhline(0, color='grey', linewidth=0.5, linestyle='--')
plt.axvline(0, color='grey', linewidth=0.5, linestyle='--')
plt.show()