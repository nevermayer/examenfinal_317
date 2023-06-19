from multiprocessing import Process, Value

def check_palindrome(word, inicio, fin, is_palindrome):
    for i in range(inicio, fin):
        if word[i] != word[len(word) - i - 1]:
            is_palindrome.value = False
            return
    is_palindrome.value = True

if __name__ == '__main__':
    #word = "oruro"
    word=input()
    word_len = len(word)
    num_processes = 2

    # Divide la palabra en partes iguales
    chunk_size = word_len // num_processes

    processes = []
    is_palindrome = Value('b', True)

    # Crea un proceso separado para cada parte
    for i in range(num_processes):
        inicio = i * chunk_size
        fin = inicio + chunk_size
        process = Process(target=check_palindrome, args=(word, inicio, fin, is_palindrome))
        processes.append(process)
        process.start()

    # Espera a que todos los procesos terminen
    for process in processes:
        process.join()

    if is_palindrome.value:
        print("Verdad")
    else:
        print("Falso")