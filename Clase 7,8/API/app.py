from flask import Flask, request, jsonify
import pyodbc

app = Flask(__name__)

# Configuración de la conexión a SQL Server con autenticación integrada de Windows
def get_db_connection():
    connection = pyodbc.connect(
        'DRIVER={ODBC Driver 17 for SQL Server};'
        'SERVER=localhost;'  # Cambia esto si tu servidor está en otra máquina o red
        'DATABASE=master;'  # Cambia por el nombre de tu base de datos
        'Trusted_Connection=yes;'  # Autenticación integrada de Windows
    )
    return connection

# Endpoint para verificar el ID del usuario y actualizar el saldo
@app.route('/verify_user', methods=['POST'])
def verify_user():
    data = request.json
    print(data)

    # Verifica que el campo 'rfid' esté presente en los datos
    if 'rfid' not in data:
        return jsonify({"error": "no se envió un rfid"}), 400

    rfid = data['rfid']

    # Conexión a la base de datos
    connection = get_db_connection()
    cursor = connection.cursor()

    # Consulta para verificar si el usuario existe con el RFID
    cursor.execute("SELECT saldo FROM Usuario WHERE rfid = ?", rfid)
    user = cursor.fetchone()

    if user:
        # El usuario existe, le restamos 3 quetzales de su saldo
        nuevo_saldo = user[0] - 3.00
        cursor.execute("UPDATE Usuario SET saldo = ? WHERE rfid = ?", nuevo_saldo, rfid)
        connection.commit()

        # Cerrar la conexión
        cursor.close()
        connection.close()

        return jsonify({"status": "success", "message": f"Saldo actualizado a {nuevo_saldo} quetzales"}), 200
    else:
        # El usuario no fue encontrado
        cursor.close()
        connection.close()
        return jsonify({"status": "fail", "message": "Usuario no encontrado"}), 404




## ENDPOINTS QUE MANEJAN EL FRONTEND
@app.route('/get_users', methods=['GET'])
def get_users():
    # Conexión a la base de datos
    connection = get_db_connection()
    cursor = connection.cursor()

    # Consulta para obtener todos los usuarios
    cursor.execute("SELECT id_usuario, nombre, tipo_usuario, rfid, saldo FROM Usuario")
    users = cursor.fetchall()

    # Formatear los resultados en una lista de diccionarios
    user_list = []
    for user in users:
        user_list.append({
            "id_usuario": user[0],
            "nombre": user[1],
            "tipo_usuario": user[2],
            "rfid": user[3],
            "saldo": user[4]
        })

    # Cerrar conexión
    cursor.close()
    connection.close()

    return jsonify(user_list)


# Endpoint para testear si el servidor funciona
@app.route('/', methods=['GET'])
def test():
    return jsonify({"status": "success", "message": "El servidor está funcionando correctamente"}), 200
if __name__ == '__main__':
    app.run(debug=True)
