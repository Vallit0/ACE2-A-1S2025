CREATE TABLE Usuario (
    id_usuario INT PRIMARY KEY,      -- ID único para cada usuario
    nombre NVARCHAR(100),            -- Nombre del usuario
    tipo_usuario NVARCHAR(50),       -- Tipo de usuario (puede ser admin, usuario, etc.)
    rfid NVARCHAR(50),               -- Código RFID del usuario
    saldo FLOAT                      -- Saldo del usuario
);
