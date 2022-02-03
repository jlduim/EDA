import random
import timeit
import statistics
import subprocess

N_NODES = 256945
N_TESTS = 500


def testar(heuristica, n_testes):

    info = []

    for n in range(n_testes):

        source, target = 0, 0

        while source == target:

            source, target = random.randint(
                0, N_NODES - 1), random.randint(0, N_NODES - 1)

        args = ("../src/back-end/testes", str(source),
                str(target), str(heuristica))

        t_inicial = timeit.default_timer()
        s = subprocess.run(args, stdout=subprocess.PIPE)
        t_final = timeit.default_timer() - t_inicial

        info.append([t_final, source, target])

    return info


def main():

    print("Testes a serem executados:", N_TESTS)
    print("Iniciando testes sem heurística")

    info = testar("0", N_TESTS)

    print()
    print(f"Tempo médio: {sum([i[0] for i in info])/len(info):.4f}")
    print(f"Desvio Padrão: {statistics.stdev([i[0] for i in info]):.4f}")

    print()
    print()

    print("Iniciando testes com heurística")

    info = testar("1", N_TESTS)

    print()
    print(f"Tempo médio: {sum([i[0] for i in info])/len(info):.4f}")
    print(f"Desvio Padrão: {statistics.stdev([i[0] for i in info]):.4f}")

    return info


if __name__ == "__main__":
    info = main()
