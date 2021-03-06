#!/bin/bash -l
#SBATCH --job-name=CUDAAssignment
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --time=0-00:05 # time (D-HH:MM)
#SBATCH --partition=gpu
#SBATCH --gres=gpu:1


export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK}
export MKL_NUM_THREADS=${SLURM_CPUS_PER_TASK}
echo 'running with OMP_NUM_THREADS =' $OMP_NUM_THREADS
echo 'running with MKL_NUM_THREADS =' $MKL_NUM_THREADS
echo "This is job '$SLURM_JOB_NAME' (id: $SLURM_JOB_ID) running on the following nodes:"
echo $SLURM_NODELIST
echo "running with OMP_NUM_THREADS= $OMP_NUM_THREADS "
echo "running with SLURM_TASKS_PER_NODE= $SLURM_TASKS_PER_NODE "

if [ ! -f Assignment2_cuda ] ; then
   echo "unable to find decay"
   echo "you probably need to compile code"
   exit 2
fi

> output_cuda.txt
for i in {4..15}; do
   mat_size=$((1<<i))
   printf "Matrix size: %36s\n" $mat_size >> output_cuda.txt
   time ./Assignment2_cuda $mat_size >> output_cuda.txt
done
