#!/usr/bin/env python3
"""
Cria o banco de dados robot_data.db (SQLite) com as tabelas:
    - imu       (timestamp_sec, timestamp_nanosec, accel_x/y/z, gyro_x/y/z, mag_x/y/z)
    - gnss      (timestamp_sec, timestamp_nanosec, latitude, longitude, elevation)
    - odometry  (timestamp_sec, timestamp_nanosec, velocity_left, velocity_right)
    - system    (timestamp_sec, timestamp_nanosec, temperature, battery_voltage)

Uso:
    python3 create_sensor_db.py [caminho/para/robot_data.db]

Se nenhum caminho for passado, cria "robot_data.db" no diretório atual.
"""

import sqlite3
import sys
from pathlib import Path

SCHEMA = {
    "imu": """
        CREATE TABLE IF NOT EXISTS imu (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp_sec INTEGER NOT NULL,
            timestamp_nanosec INTEGER NOT NULL,
            accel_x INTEGER,
            accel_y INTEGER,
            accel_z INTEGER,
            gyro_x INTEGER,
            gyro_y INTEGER,
            gyro_z INTEGER,
            mag_x INTEGER,
            mag_y INTEGER,
            mag_z INTEGER
        );
    """,
    "gnss": """
        CREATE TABLE IF NOT EXISTS gnss (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp_sec INTEGER NOT NULL,
            timestamp_nanosec INTEGER NOT NULL,
            latitude REAL,
            longitude REAL,
            elevation REAL
        );
    """,
    "odometry": """
        CREATE TABLE IF NOT EXISTS odometry (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp_sec INTEGER NOT NULL,
            timestamp_nanosec INTEGER NOT NULL,
            velocity_left INTEGER,
            velocity_right INTEGER
        );
    """,
    "system": """
        CREATE TABLE IF NOT EXISTS system (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp_sec INTEGER NOT NULL,
            timestamp_nanosec INTEGER NOT NULL,
            temperature INTEGER,
            battery_voltage INTEGER
        );
    """,
}

# Índices por timestamp, úteis para consultas por intervalo de tempo
INDEXES = {
    "imu": "CREATE INDEX IF NOT EXISTS idx_imu_timestamp ON imu (timestamp_sec, timestamp_nanosec);",
    "gnss": "CREATE INDEX IF NOT EXISTS idx_gnss_timestamp ON gnss (timestamp_sec, timestamp_nanosec);",
    "odometry": "CREATE INDEX IF NOT EXISTS idx_odometry_timestamp ON odometry (timestamp_sec, timestamp_nanosec);",
    "system": "CREATE INDEX IF NOT EXISTS idx_system_timestamp ON system (timestamp_sec, timestamp_nanosec);",
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
    db_path = sys.argv[1] if len(sys.argv) > 1 else "robot_data.db"
    create_database(db_path)
