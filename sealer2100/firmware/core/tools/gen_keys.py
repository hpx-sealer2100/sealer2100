from cryptography.hazmat.primitives.asymmetric.ed25519 import Ed25519PrivateKey
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import ec

# Generate a new Ed25519 private key
private_key = Ed25519PrivateKey.generate()

# Get the public key from the private key
public_key = private_key.public_key()

# Convert keys to hexadecimal format

private_key_hex = private_key.private_bytes(
    encoding=serialization.Encoding.Raw,
    format=serialization.PrivateFormat.Raw,
    encryption_algorithm=serialization.NoEncryption()
).hex()

public_key_hex = public_key.public_bytes(
    encoding=serialization.Encoding.Raw,
    format=serialization.PublicFormat.Raw
).hex()

# Print the keys
print(f"固件私钥(Hex): {private_key_hex}")
print(f"固件公钥(Hex): {public_key_hex}")

print("=========================================")
print("=========================================")


# Generate a new P-256 (secp256r1) private key
private_key_256r1 = ec.generate_private_key(ec.SECP256R1())

# Get the public key from the private key
public_key_256r1 = private_key_256r1.public_key()

# Convert keys to hexadecimal format
private_key_256r1_hex = private_key_256r1.private_numbers().private_value.to_bytes(
    32, byteorder='big'
).hex()

public_key_256r1_hex = public_key_256r1.public_numbers().x.to_bytes(
    32, byteorder='big'
).hex() + public_key_256r1.public_numbers().y.to_bytes(
    32, byteorder='big'
).hex()

# Print the keys
print(f"蓝牙私钥(Hex): {private_key_256r1_hex}")
print(f"蓝牙公钥(Hex): {public_key_256r1_hex}")


