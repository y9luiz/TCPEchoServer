import socket

# Função para calcular o tamanho da mensagem
def calcular_tamanho_mensagem(campos):
    tamanho = 0
    for campo in campos:
        tamanho += len(campo)
    return tamanho.to_bytes(2, byteorder='big')

# Inicialização dos campos
tipo_mensagem = (0).to_bytes(1, byteorder='big')
message_sequence = (1).to_bytes(1, byteorder='big')
username = "Luiz".encode('utf-8').ljust(32, b'\x00')
password = "password".encode('utf-8').ljust(32, b'\x00')

# Montagem do bytearray
tamanho_mensagem = calcular_tamanho_mensagem([tipo_mensagem, message_sequence, username, password])
byte_array = bytearray(tamanho_mensagem + tipo_mensagem + message_sequence + username + password)

# Exemplo de uso
print(byte_array)


clientsocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
clientsocket.connect(('localhost', 5000))
print("ok")
message = bytes([])

clientsocket.send(byte_array)

reply = clientsocket.recv(5)
print (reply)