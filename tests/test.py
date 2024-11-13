import requests
import concurrent.futures
import time

# Конфигурация
URL_ENCRYPT = "http://localhost:8080/encrypt"
URL_DECRYPT = "http://localhost:8080/decrypt"
BASE_TEXT = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. " * 10
SHIFT = 3
NUM_REQUESTS = 1_000_000  # Количество запросов для стресс-теста
CONCURRENCY = 100   # Количество параллельных потоков

current_text = BASE_TEXT
count = 0;

def encrypt_request():
    """Отправляет запрос на шифрование с увеличивающимся текстом"""
    global current_text
    params = {"text": current_text, "shift": SHIFT}
    # current_text += "*";
    response = requests.get(URL_ENCRYPT, params=params)
    return response.status_code, response.text

def decrypt_request():
    """Отправляет запрос на дешифрование с увеличивающимся текстом"""
    global current_text
    params = {"text": current_text, "shift": SHIFT}
    # current_text += "*";
    response = requests.get(URL_DECRYPT, params=params)
    return response.status_code, response.text

def run_stress_test():
    """Запускает стресс-тестирование для шифрования и дешифрования"""
    start_time = time.time()

    with concurrent.futures.ThreadPoolExecutor(max_workers=CONCURRENCY) as executor:
        encrypt_futures = [executor.submit(encrypt_request) for _ in range(NUM_REQUESTS // 2)]
        decrypt_futures = [executor.submit(decrypt_request) for _ in range(NUM_REQUESTS // 2)]

        for i, future in enumerate(concurrent.futures.as_completed(encrypt_futures + decrypt_futures)):
            status_code, response_text = future.result()
            if status_code != 200:
                print(f"Ошибка: статус {status_code}, ответ: {response_text}")
    
    end_time = time.time()
    print(f"Стресс-тест завершен. Время выполнения: {end_time - start_time:.2f} секунд")

if __name__ == "__main__":
    run_stress_test()
