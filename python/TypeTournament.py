import numpy as np
import matplotlib.pyplot as plt
from itertools import combinations_with_replacement

# Define type names
types = ['NOR', 'DAR', 'PSY', 'WAT', 'FIR', 'GRA', 'FRO', 'GRO', 'STE', 'WIN', 'ELE', 'DGT']

# Raw matchup table: attacker (rows) vs defender (columns)
matchUp = np.array([
    [1.0,2.0,0.5,1.0,1.0,1.0,1.0,1.0,0.5,1.0,1.0,2.0],  # Normal
    [0.5,0.5,2.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0],  # Dark
    [2.0,0.5,0.5,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0],  # Psychic
    [1.0,1.0,1.0,0.5,2.0,0.5,0.5,1.0,1.0,1.0,0.5,1.0],  # Water
    [1.0,1.0,1.0,0.5,0.5,2.0,2.0,0.5,2.0,0.5,1.0,1.0],  # Fire
    [1.0,1.0,1.0,2.0,0.5,0.5,0.5,2.0,0.5,1.0,1.0,1.0],  # Grass
    [1.0,1.0,1.0,2.0,0.5,2.0,0.5,1.0,1.0,1.0,1.0,1.0],  # Frost
    [1.0,1.0,1.0,0.5,2.0,0.5,1.0,0.5,1.0,1.0,2.0,1.0],  # Ground
    [2.0,1.0,1.0,1.0,0.5,2.0,1.0,1.0,0.5,2.0,0.5,0.5],  # Steel
    [1.0,1.0,1.0,1.0,2.0,1.0,1.0,1.0,0.5,0.5,1.0,1.0],  # Wind
    [1.0,1.0,1.0,2.0,1.0,1.0,1.0,0.5,2.0,1.0,0.5,0.5],  # Electric
    [1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,2.0,1.0,2.0,0.5],  # Digital
])

num_types = len(types)
results = []

for combo in combinations_with_replacement(range(num_types), 2):
    t1, t2 = combo
    name = f"{types[t1]}" if t1 == t2 else f"{types[t1]}+{types[t2]}"
    effectiveness = 0
    for i in range(num_types):
        effectiveness += matchUp[i][t1]* matchUp[i][t2] if t1 != t2 else matchUp[i][t1]
    # effectiveness = matchUp[t1] * matchUp[t2] if t1 != t2 else matchUp[t1]
    # avg_effectiveness = np.mean(effectiveness)
    avg_effectiveness = effectiveness / num_types
    results.append((name, avg_effectiveness))

results.sort(key=lambda x: x[1], reverse=True)
names, scores = zip(*results)

plt.figure(figsize=(8, 16))
plt.barh(range(len(scores)), scores, tick_label=names)
plt.xlabel("Average Winrate")
plt.title("Average Winrate of All Type Combinations")
plt.tight_layout()
plt.show()