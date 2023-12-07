import sqlite3
from datetime import datetime

class DBStorage:
    def __init__(self, db_name="data.db"):
        self.db_name = db_name
        self.db = None
        self.cursor = None

    def connect(self):
        self.db = sqlite3.connect(self.db_name)
        self.cursor = self.db.cursor()

    def disconnect(self):
        self.db.close()

    def create_table(self):
        self.cursor.execute(
            "CREATE TABLE IF NOT EXISTS mediciones (id INTEGER PRIMARY KEY AUTOINCREMENT, valor_temperatura FLOAT, valor_humedad FLOAT, fecha TIMESTAMP DEFAULT CURRENT_TIMESTAMP)")

    def insert(self, humedad, temperatura):
        fecha_actual = datetime.now().strftime("%Y-%m-%d %H:%M:%S")  # Obtiene la fecha y hora actual
        self.cursor.execute(
            "INSERT INTO mediciones (valor_humedad, valor_temperatura, fecha) VALUES (?, ?, ?)", (humedad, temperatura, fecha_actual))
        self.db.commit()

    def get_measurements(self, fecha_inicio, fecha_fin):
        query = "SELECT * FROM mediciones WHERE fecha BETWEEN ? AND ?"
        self.cursor.execute(query, (fecha_inicio, fecha_fin))
        labels = []
        temperatures = []
        humidities = []

        for row in self.cursor.fetchall():
            labels.append(row[3])
            temperatures.append(row[1])
            humidities.append(row[2])

        return {
            "labels": labels,
            "temperatures": temperatures,
            "humidities": humidities
        }

if __name__ == "__main__":
    db = DBStorage("data.db")
    db.connect()
    db.create_table()
    db.insert(10, 20)
    print(db.get_measurements())
    db.disconnect()
