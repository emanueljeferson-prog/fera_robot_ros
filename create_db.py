#!/usr/bin/env python3
"""
Cria o banco de dados robot_data.db (SQLite) com as tabelas:
    - imu       (timestamp_sec, timestamp_nanosec, accel_x/y/z, gyro_x/y/z, mag_x/y/z)
    - gnss      (timestamp_sec, timestamp_nanosec, latitude, longitude, elevation)
    - odometry  (timestamp_sec, timestamp_nanosec, velocity_left, velocity_right)
    - system    (timestamp_sec, timestamp_nanosec, temperature, battery_voltage)
    - configurations (uma coluna para cada valor de configurations.json)

Uso:
    python3 create_sensor_db.py [caminho/para/robot_data.db]

Se nenhum caminho for passado, cria "robot_data.db" no diretório atual.
"""

import json
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
    "configurations": """
        CREATE TABLE IF NOT EXISTS configurations (
            id INTEGER PRIMARY KEY CHECK (id = 1),
            accel_offset_x REAL NOT NULL,
            accel_offset_y REAL NOT NULL,
            accel_offset_z REAL NOT NULL,
            gyro_offset_x REAL NOT NULL,
            gyro_offset_y REAL NOT NULL,
            gyro_offset_z REAL NOT NULL,
            mag_offset_x REAL NOT NULL,
            mag_offset_y REAL NOT NULL,
            mag_offset_z REAL NOT NULL,
            accel_scale REAL NOT NULL,
            gyro_scale REAL NOT NULL,
            mag_scale REAL NOT NULL,
            gnss_reference_lat REAL NOT NULL,
            gnss_reference_lon REAL NOT NULL,
            gnss_reference_alt REAL NOT NULL,
            semi_major_axis REAL NOT NULL,
            semi_minor_axis REAL NOT NULL,
            eccentricity REAL NOT NULL,
            flattening REAL NOT NULL,
            gravity REAL NOT NULL,
            magnetic_declination REAL NOT NULL,
            magnetic_inclination REAL NOT NULL,
            magnetic_field_strength REAL NOT NULL,
            scale_factor_encoder REAL NOT NULL
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

CONFIGURATIONS_PATH = Path(__file__).with_name("configurations.json")


def create_database(db_path: str) -> None:
    path = Path(db_path)
    path.parent.mkdir(parents=True, exist_ok=True)

    conn = sqlite3.connect(str(path))
    try:
        cursor = conn.cursor()
        for table_name, create_stmt in SCHEMA.items():
            cursor.execute(create_stmt)
            if table_name in INDEXES:
                cursor.execute(INDEXES[table_name])
            print(f"[OK] Tabela '{table_name}' criada/verificada.")

        with CONFIGURATIONS_PATH.open(encoding="utf-8") as config_file:
            configurations = json.load(config_file)
        if not isinstance(configurations, dict):
            raise ValueError("configurations.json deve conter um objeto JSON.")

        configuration_columns = tuple(configurations)
        table_columns = tuple(
            column[1]
            for column in cursor.execute("PRAGMA table_info(configurations)")
            if column[1] != "id"
        )
        if configuration_columns != table_columns:
            raise ValueError(
                "Os campos de configurations.json não correspondem às colunas "
                "da tabela configurations."
            )

        columns = ", ".join(configuration_columns)
        placeholders = ", ".join("?" for _ in configuration_columns)
        updates = ", ".join(
            f"{column} = excluded.{column}" for column in configuration_columns
        )
        cursor.execute(
            f"""
            INSERT INTO configurations (id, {columns})
            VALUES (1, {placeholders})
            ON CONFLICT(id) DO UPDATE SET {updates};
            """,
            tuple(configurations.values()),
        )
        print(f"[OK] {len(configurations)} configurações carregadas.")
        conn.commit()
    finally:
        conn.close()

    print(f"\n✅ Banco de dados criado em: {path.resolve()}")


if __name__ == "__main__":
    db_path = sys.argv[1] if len(sys.argv) > 1 else "robot_data.db"
    create_database(db_path)
