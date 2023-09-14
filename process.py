import argparse
import numpy as np
from datetime import datetime
import matplotlib.pyplot as plt

res = {}
benchmarks = []

parser = argparse.ArgumentParser(description="Generate benchmark results and graphs");
parser.add_argument("in_file", 
					metavar="INPUT_FILE",
					type=str, 
					help="Path to file containing raw benchmarks")
parser.add_argument("out_file", 
					metavar="OUTPUT_FILE",
					type=str, 
					help="Path to intended output file")
parser.add_argument("platform",
					choices=["sel4", "linux"],
					help="Which platform these benchmarks are coming from");

args = parser.parse_args();

with open(args.in_file) as in_file:
	while (line := in_file.readline().rstrip('\n')):
		print(line)
		if line == "## BEGIN benchmark results ##":
			break

	if (line != "## BEGIN benchmark results ##"):
		print("Could not find locate benchmarks in input")
		exit(1)

	while ((line := in_file.readline().rstrip('\n')) and
		   line != "## END benchmark results ##"):
	
		benchmark_name = line
		benchmarks.append(benchmark_name)

		benchmark_results_str = in_file.readline().rstrip(", \n").split(',')
		benchark_results = [int(x.strip()) for x in benchmark_results_str]
		res[benchmark_name] = {'mean': round(np.mean(benchark_results), 2),
							   'std_dev': round(np.std(benchark_results), 2)}


with open(args.out_file, 'w+') as out_file:
	out_file.write("benchmark, mean, std-dev\n")
	for benchmark in benchmarks:
		out_file.write(f"{benchmark}, {res[benchmark]['mean']}, {res[benchmark]['std_dev']}")

x_pos = np.arange(len(benchmarks))
values = [res[x]['mean'] for x in benchmarks]
err = [res[x]['std_dev'] for x in benchmarks]

plt.bar(x_pos, values, yerr=err, tick_label = benchmarks, color='green',
		align='center', alpha=0.5, ecolor='black', capsize=10)
plt.ylabel("cycles")
plt.tight_layout()
plt.title(f"Results from input file '{args.in_file}' for {args.platform}")
plt.show()


