import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV file
df = pd.read_csv("runtimes.csv")

# Extract the second part of graph_dim (the number of edges)
df["edges"] = df["graph_dim"].apply(lambda x: int(x.split()[1]))

# Sort by number of edges to keep the x-axis ordered
df = df.sort_values(by="edges")

# Plot
plt.figure(figsize=(10, 6))
for program in df["program"].unique():
    program_df = df[df["program"] == program]
    plt.plot(program_df["edges"], program_df["runtime"], marker='o', label=program)

plt.xlabel("Number of Nodes (Edges ≈ ½ × fully connected graph)")
plt.ylabel("Runtime (seconds)")
plt.title("Runtime vs Graph Size by Program")
plt.legend(title="Program")
plt.grid(True)
plt.tight_layout()
plt.show()

