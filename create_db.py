#!/usr/bin/env python3
"""
Cria o banco de dados sensor_data.db (SQLite) com as tabelas:
  - imu       (timestamp, accel_x/y/z, gyro_x/y/z, mag_x/y/z)
  - gnss      (timestamp, latitude, longitude, elevation)
  - odometry  (timestamp, velocity_left, velocity_right)
  - system    (timestamp, temperature, battery_voltage)

Uso:
    python3 create_sensor_db.py [caminho/para/sensor_data.db]

Se nenhum caminho for passado, cria "sensor_data.db" no diretório atual.
"""

import sqlite3
import sys
from pathlib import Path

SCHEMA = {
    "imu": """
        CREATE TABLE IF NOT EXISTS imu (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp REAL NOT NULL,
            accel_x REAL,
            accel_y REAL,
            accel_z REAL,
            gyro_x REAL,
            gyro_y REAL,
            gyro_z REAL,
            mag_x REAL,
            mag_y REAL,
            mag_z REAL
        );
    """,
    "gnss": """
        CREATE TABLE IF NOT EXISTS gnss (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp REAL NOT NULL,
            latitude REAL,
            longitude REAL,
            elevation REAL
        );
    """,
    "odometry": """
        CREATE TABLE IF NOT EXISTS odometry (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp REAL NOT NULL,
            velocity_left REAL,
            velocity_right REAL
        );
    """,
    "system": """
        CREATE TABLE IF NOT EXISTS system (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp REAL NOT NULL,
            temperature REAL,
            battery_voltage REAL
        );
    """,
}

# Índices por timestamp, úteis para consultas por intervalo de tempo
INDEXES = {
    "imu": "CREATE INDEX IF NOT EXISTS idx_imu_timestamp ON imu (timestamp);",
    "gnss": "CREATE INDEX IF NOT EXISTS idx_gnss_timestamp ON gnss (timestamp);",
    "odometry": "CREATE INDEX IF NOT EXISTS idx_odometry_timestamp ON odometry (timestamp);",
    "system": "CREATE INDEX IF NOT EXISTS idx_system_timestamp ON system (timestamp);",
}


def create_database(db_path: str) -> None:
    path = Path(db_path)
    path.parent.mkdir(parents=True, exist_ok=True)

    conn = sqlite3.connect(str(path))
    try:
        cursor = conn.cursor()
        for table_name, create_stmt in SCHEMA.items():
            cursor.execute(create_stmt)
            cursor.execute(INDEXES[table_name])
            print(f"[OK] Tabela '{table_name}' criada/verificada.")
        conn.commit()
    finally:
        conn.close()

    print(f"\n✅ Banco de dados criado em: {path.resolve()}")


if __name__ == "__main__":
    db_path = sys.argv[1] if len(sys.argv) > 1 else "sensor_data.db"
    create_database(db_path)
