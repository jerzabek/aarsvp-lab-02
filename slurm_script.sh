#!/bin/bash
#SBATCH --job-name=aarsvp_lab02_0036526653
#SBATCH --time=00:10:00
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=32
#SBATCH --mem-per-cpu=256MB
#SBATCH -o omp-%j.out
#SBATCH --partition=computes_thin

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

./main rgb_video.yuv
