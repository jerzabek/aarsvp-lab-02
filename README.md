# AARSVP (HPC)

## Druga laboratorijska vježba

Datoteka `sequential.c` sadrži sekvencijalnu verziju koda, dok `main.c` sadrži paraleliziranu verziju koda sa OpenMP direktivama.

Izlazne datoteke nakon pokretanja programa su `output-raw.yuv`, `output-subsample.yuv` te `output-upsample.yuv`

Pretpostavljeno je da ulazna datoteka ima veličinu 3840x2160 u formatu zadanom [u zadatku](./AARSVP_lab2.pdf).

### Compile

```sh
$ gcc src/main.c -fopenmp -o main
```

### Run

```sh
$ ./main.exe video/rgb_video.yuv
```

Pokretanje pomoću SLURM okruženja (potrebno za UNIRI Bura superračunalo)

```sh
$ sbatch slurm_script.sh video/rgb_video.yuv
```