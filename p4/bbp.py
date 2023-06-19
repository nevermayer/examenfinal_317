import multiprocessing
from mpmath import mp

def calculate_pi_chunk(start, end):
    mp.dps = end - start + 1  # Establecer la precisión deseada para el cálculo de los dígitos de Pi
    pi = mp.mpf(0)  # Inicializar el valor de Pi para la porción de dígitos procesados
    for k in range(start, end + 1):
        # Fórmula de Bailey-Borwein-Plouffe para el cálculo de dígitos de Pi
        pi += (mp.mpf(1)/(16**k))*(
            mp.mpf(4)/(8*k+1) - mp.mpf(2)/(8*k+4) - mp.mpf(1)/(8*k+5) - mp.mpf(1)/(8*k+6)
        )
    return pi

def calculate_pi(digits):
    num_processes = multiprocessing.cpu_count()  # Obtener el número de procesadores disponibles
    chunk_size = digits // num_processes  # Dividir los dígitos entre los procesos
    processes = []  # Lista para almacenar los procesos
    results = multiprocessing.Queue()  # Cola para almacenar los resultados parciales

    for i in range(num_processes):
        start = i * chunk_size
        end = start + chunk_size - 1
        if i == num_processes - 1:  # El último proceso se encarga de los dígitos restantes
            end = digits - 1
        # Crear un proceso que calcule una porción de los dígitos de Pi y guarde el resultado en la cola
        process = multiprocessing.Process(target=lambda q, start, end: q.put(calculate_pi_chunk(start, end)),
                                          args=(results, start, end))
        processes.append(process)  # Agregar el proceso a la lista
        process.start()  # Iniciar el proceso

    for process in processes:
        process.join()  # Esperar a que todos los procesos terminen

    pi = mp.mpf(0)
    while not results.empty():
        pi += results.get()  # Obtener los resultados parciales de la cola y acumularlos

    return pi

# Ejemplo de uso
num_digits = 1000
mp.dps = num_digits + 10  # Establecer una precisión ligeramente mayor para cálculos precisos
pi = calculate_pi(num_digits)  # Calcular los dígitos de Pi
pi_str = mp.nstr(pi, num_digits)  # Convertir el resultado en una cadena de texto con la cantidad deseada de dígitos
print(f"Pi: {pi_str}")
