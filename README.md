# Ganesha Algorithmic Transpiler Engine (GATE)

Transpiler untuk mengubah Notasi Algoritmik (AN) menjadi kode C.

## Membangun Transpiler
Jalankan `make` untuk membangun executable `gate`.

## Menjalankan Transpiler
Gunakan perintah `./gate <input.notal> -o <output.c>`.

## Mengkompilasi Kode C yang Dihasilkan
Gunakan `gcc -o program output/example.c adt/adt_list.c adt/adt_stack.c`.

## Struktur Proyek
- `include/`: Header files untuk kelas-kelas utama.
- `src/`: Implementasi sumber C++.
- `adt/`: Implementasi ADT dalam C.
- `input/`: Contoh input AN.
- `output/`: Hasil kode C.