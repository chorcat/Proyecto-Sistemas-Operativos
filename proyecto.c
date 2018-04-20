#include <linux/sched.h>
#include <linux/proyecto.h>
#include <linux/signal.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/unistd.h>
#include <linux/string.h>
#include <asm/cputime.h>
#include<linux/errno.h>
#include <linux/time.h>



int num_proc_pagos = 0;
int num_proc_anul = 0;
int num_proc_reser = 0;
int num_proc_grayev = 0;
struct task_struct *proc_menosp;

void poner_cola(struct task_struct *proc){
  
  struct sched_param prioridad;
    
  if(!(strcmp(proc->comm,PAGOS))){
    prioridad.sched_priority = MAX_RT_PRIO-2;
    if(sched_setscheduler(proc, SCHED_RR, &prioridad) < 0){
      printk("\nSe ha producido un error "); 
      return;  
    } 
    proc->rt.time_slice = 8000 * HZ / 1000;
    num_proc_pagos++;
    printk("\nSe encola un proceso de PAGOS con PID %d y prioridad  %u \n", proc->pid,proc->rt_priority);
    printk("\nNumero de procesos de PAGOS: %d", num_proc_pagos); 
  } 
  if(!(strcmp(proc->comm,ANULACIONES ))){
    prioridad.sched_priority = MAX_RT_PRIO-3;
    if(sched_setscheduler(proc, SCHED_RR, &prioridad) < 0) {
      printk("\nSe ha producido un error");
      return;
    }
    num_proc_anul++;
    printk("\nSe encola un proceso de ANULACIONES con PID %d y prioridad %u\n", proc->pid, proc->rt_priority);
    printk("\nNumero de procesos de ANULACIONES: %d", num_proc_anul);
    proc_menosp= menos_prioritario(proc_menosp, proc);
  }
  if(!(strcmp(proc->comm,RESERVAS))){
    prioridad.sched_priority = MAX_RT_PRIO-4;
    if(sched_setscheduler(proc, SCHED_RR, &prioridad) < 0) {
      printk("\nSe ha producido un error en %d", proc->pid);
      return;
    }
    num_proc_reser++;
    printk("\nSe encola un proceso de RESERVAS con PID %d y prioridad %u\n", proc->pid, proc->rt_priority);
    printk("\nNumero de procesos de RESERVAS: %d", num_proc_reser);
    proc_menosp= menos_prioritario(proc_menosp, proc);
    
  }
  if(!(strcmp(proc->comm,GRADAS))||!(strcmp(proc->comm,EVENTOS))){
    prioridad.sched_priority = MAX_RT_PRIO-5;
    if(sched_setscheduler(proc, SCHED_RR, &prioridad) < 0){
      printk("Se ha producido un error en %d", proc->pid);  
      return;
    }
    num_proc_grayev++;	
    printk("\nSe encola un proceso de %s con PID %d y prioridad %u\n", proc->comm, proc->pid, proc->rt_priority);
    printk("\nNumero de procesos de GRADAS y EVENTOS: %d\n", num_proc_grayev);
    proc_menosp= menos_prioritario(proc_menosp, proc);
  }
}


void contar_eliminados(struct task_struct *proc){
  if (!(strcmp(proc->comm,PAGOS))){
    num_proc_pagos--;
    printk("\nSe ha eliminado un proceso con pid %d de %s (ha consumido %lu segundos), quedan %d procesos de en el sistema.",proc->pid, proc->comm, cputime_to_secs(proc->utime), num_proc_pagos+num_proc_anul+num_proc_reser+num_proc_grayev);
  }
  else if (!(strcmp(proc->comm,ANULACIONES))){
    num_proc_anul--;
 printk("\nSe ha eliminado un proceso con pid %d de %s (ha consumido %lu segundos), quedan %d procesos de en el sistema.",proc->pid, proc->comm, cputime_to_secs(proc->utime), num_proc_pagos+num_proc_anul+num_proc_reser+num_proc_grayev);
  }
  else if (!(strcmp(proc->comm,RESERVAS))){
    num_proc_reser--;
   printk("\nSe ha eliminado un proceso con pid %d de %s (ha consumido %lu segundos), quedan %d procesos de en el sistema.",proc->pid, proc->comm, cputime_to_secs(proc->utime), num_proc_pagos+num_proc_anul+num_proc_reser+num_proc_grayev);
  }
  else if (!(strcmp(proc->comm,GRADAS))||!(strcmp(proc->comm,EVENTOS))){
    num_proc_grayev--;
  printk("\nSe ha eliminado un proceso con pid %d de %s (ha consumido %lu segundos), quedan %d procesos de en el sistema.",proc->pid, proc->comm, cputime_to_secs(proc->utime), num_proc_pagos+num_proc_anul+num_proc_reser+num_proc_grayev);
  } 
}

struct task_struct * menos_prioritario(struct task_struct *proc_old,struct task_struct *proc_new){
  if(proc_menosp == NULL) {
    return proc_new;
  }
  if(!(strcmp(proc_new->comm,GRADAS))||!(strcmp(proc_new->comm,EVENTOS))){
    return proc_new;
  }
  if(!(strcmp(proc_old->comm,GRADAS))||!(strcmp(proc_old->comm,EVENTOS))){
    
    return proc_old;
  }
  if(!(strcmp(proc_new->comm,RESERVAS))){
    return proc_new;
  }
  if(!(strcmp(proc_old->comm,RESERVAS))){
    return proc_old;
  }
  
  if(!(strcmp(proc_new->comm,ANULACIONES))){
    return proc_new;
  }
  return proc_old; 
  
}

void controlar_tiempo(struct task_struct *proc) {
  if(!strcmp(proc->comm, PAGOS)) {
    if(cputime_to_secs(proc->utime) >= 24) {
      kill_pid(find_vpid(proc->pid), SIGKILL, 1);
    }
  }
  else if(!strcmp(proc->comm, ANULACIONES)) {
    if(cputime_to_secs(proc->utime) >= 10) {
      kill_pid(find_vpid(proc->pid), SIGKILL, 1);
    }
  }
  else if(!strcmp(proc->comm, RESERVAS)) {
    if(cputime_to_secs(proc->utime) >= 10) {
      kill_pid(find_vpid(proc->pid), SIGKILL, 1);
    }
  }
  else if(!strcmp(proc->comm, EVENTOS)) {
    if(cputime_to_secs(proc->utime) >= 10) {
      kill_pid(find_vpid(proc->pid), SIGKILL, 1);
    }
  }
  else if(!strcmp(proc->comm, GRADAS)) {
    if(cputime_to_secs(proc->utime) >= 10) {
      kill_pid(find_vpid(proc->pid), SIGKILL, 1);
    }
  }
}

int validar_requisitos(struct task_struct *proc) {
  printk("\nHay %d procesos en el sistema.", num_proc_pagos+num_proc_anul+num_proc_reser+num_proc_grayev);

  // Se comprueba si se le permite la entrada si hay mas de 10 procesos en el sistema
  if((num_proc_pagos+num_proc_anul+num_proc_reser+num_proc_grayev) >= 10) {

    if(!strcmp(proc->comm, PAGOS) && num_proc_pagos < 10) {
      printk("\nSe ha descartado un proceso de %s con PID %d a favor de un proceso de PAGOS con PID %d\n", proc_menosp->comm, proc_menosp->pid, proc->pid);
      kill_pid(find_vpid(proc_menosp->pid),SIGKILL,1); //liberamos espacio
      return 1;
    }

    else if(!strcmp(proc->comm,ANULACIONES) && num_proc_pagos+num_proc_anul<10){
            printk("\nSe ha descartado un proceso de %s con PID %d a favor de un proceso de ANULACIONES con PID %d\n", proc_menosp->comm, proc_menosp->pid, proc->pid);
      kill_pid(find_vpid(proc_menosp->pid),SIGKILL,1);
      return 1;
    }

    else if(!strcmp(proc->comm, RESERVAS) && num_proc_pagos+num_proc_anul+num_proc_reser < 10) {
      printk("\nSe ha descartado un proceso de %s con PID %d a favor de un proceso de RESERVAS con PID %d\n", proc_menosp->comm, proc_menosp->pid, proc->pid);
      kill_pid(find_vpid(proc_menosp->pid),SIGKILL,1);
      return 1;
    }

    // Si es de gradas o eventos no se le permite entrar
    else {
            printk("\nNo se ha permitido la entrada al proceso %s con PID %d porque la memoria esta llena con procesos de mayor o igual prioridad", proc->comm, proc->pid);
           
		
     kill_pid(find_vpid(proc->pid), SIGKILL, 1) ;
      return 0;
    }
  }

  // Si hay menos de 10 procesos en el sistema se le permite entrar
  return 1;
}
  
