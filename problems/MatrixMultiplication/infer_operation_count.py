import pandas as pd
import matplotlib.pyplot as plt

#The runtime data obtained is used in order to see how the runtimes change with matrix sizes. For this, we consider the number of columns of 'A' as a measure against which the runtime can be measured.

#Requires runtimes.csv to be present which in turn requires the get_runtimes.sh file to be run
df = pd.read_csv('runtimes.csv')


df[['m', 'k', 'n', 'o']] = df['matrix_size'].str.split(' ', expand=True).astype(int)        #splitting matrix size into m, k, n, o  -  rows_a, cols_a, rows_b, cols_b


df['operations'] = df['n']                          #column size of 'a' is our x-axis for the plot.
#df['operations']= df['o']                          #when the data is plotted with changing column size of 'b'.
df['runtime_ms'] = df['runtime'] * 1000             #converting from seconds to milliseconds.
df = df.sort_values('operations')                   #sorting by operation count.


plt.figure(figsize=(12, 8))

#plotting each program separately
for program in df['program'].unique():
    program_data = df[df['program'] == program]
    plt.plot(program_data['operations'], program_data['runtime_ms'], marker='o', label=program)


plt.xlabel('Number of Columns')
plt.ylabel('Runtime (milliseconds)')
plt.title('Matrix Multiplication Runtime vs Operations')
plt.xscale('linear')          
plt.yscale('linear')          

plt.grid(True, which='both', linestyle='--', linewidth=0.5)
plt.legend()
plt.tight_layout()
plt.show()
