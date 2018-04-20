#include <linux/sched.h>

#define PAGOS "pagos"
#define RESERVAS "reservas"
#define ANULACIONES "anulaciones"
#define GRADAS "gradas"
#define EVENTOS "eventos"

void poner_cola(struct task_struct *proc);
void controlar_tiempo(struct task_struct *proc);
void contar_eliminados(struct task_struct *proc);
int validar_requisitos(struct task_struct *proc);
struct task_struct *  menos_prioritario(struct task_struct *proc_old, struct task_struct *proc_new);
