#include <videoDriver.h>
#include <keyboard.h>
#include <lib.h>
#include <time.h>
#include <sound.h>
#include <memoryManager.h>
#include <scheduler.h>
#include <semaphore.h>
#include <stdint.h>
#include <pipe.h>


#define STDIN 0
#define STDOUT 1
#define STDERR 2

extern uint8_t hasregisterInfo;
extern const uint64_t registerInfo[17];
extern int _hlt();

extern Color WHITE;
extern Color BLACK;

static uint64_t sys_read(uint64_t fd)
{
    if (fd != STDIN)
    {
        return -1;
    }

    return readKeyboard();
}

static uint64_t sys_drawCursor()
{
    vDriver_drawCursor();
    return 1;
}

static uint64_t sys_write(uint64_t fd, char buffer)
{
    if (fd !=  STDOUT)
    {
        return -1;
    }

    vDriver_print(buffer, WHITE, BLACK);
    return 1;
}

static uint64_t sys_writeColor(uint64_t fd, char buffer, Color color)
{
    if (fd != STDOUT)
    {
        return -1;
    }

    vDriver_print(buffer, color, BLACK);
    return 1;
}

static uint64_t sys_clear()
{
    vDriver_clear();
    return 1;
}

static uint64_t sys_getScrHeight()
{
    return vDriver_getHeight();
}

static uint64_t sys_getScrWidth()
{
    return vDriver_getWidth();
}

static void sys_drawRectangle(int x, int y, int x2, int y2, Color color)
{
    vDriver_drawRectangle(x, y, x2, y2, color);
}

static void sys_wait(int ms)
{
    timer_wait(ms);
}

static uint64_t sys_getHours()
{
    return getHours();
}

static uint64_t sys_getMinutes()
{
    return getMinutes();
}

static uint64_t sys_getSeconds()
{
    return getSeconds();
}

static uint64_t sys_registerInfo(uint64_t registers[17])
{
    if (hasregisterInfo)
    {
        for (uint8_t i = 0; i < 17; i++)
        {
            registers[i] = registerInfo[i];
        }
    }
    return hasregisterInfo;
}

static uint64_t sys_printmem(uint64_t *address)
{
    if ((uint64_t)address > (0x20000000 - 32))
    {
        return -1;
    }

    uint8_t *aux = (uint8_t *)address;
    vDriver_prints("\n", WHITE, BLACK);
    for (int i = 0; i < 32; i++)
    {
        vDriver_printHex((uint64_t)aux, WHITE, BLACK);
        vDriver_prints(" = ", WHITE, BLACK);
        vDriver_printHex(*aux, WHITE, BLACK);
        vDriver_newline();
        aux++;
    }

    return 0;
}

static uint64_t sys_pixelPlus()
{
    plusScale();
    return 1;
}

static uint64_t sys_pixelMinus()
{
    minusScale();
    sys_clear();
    return 1;
}

static uint64_t sys_playSpeaker(uint32_t frequence, uint64_t duration)
{
    beep(frequence, duration);
    return 1;
}

static uint64_t sys_stopSpeaker()
{
    stopSpeaker();
    return 1;
}

//syscalls para semaforos
static int sys_sem_open(char * name, uint64_t init_value){
    return sem_open(name, init_value);
}
static void sys_sem_close(char * sem){
    sem_close(sem);
}
static void sys_sem_wait(char * sem){
    sem_wait(sem);
}
static void sys_sem_post(char * sem){
    sem_post(sem);
}



static void sys_wait_pid(uint64_t pid){
    waitPid(pid);
}

static void sys_mem_init(void *ptr, int size){
    return mem_init(ptr, size);
}

static void * sys_mem_alloc(uint64_t size){
    return mem_alloc(size);
}

static void sys_mem_free(void * ptr){
    return mem_free(ptr);
}

static uint64_t s_create_process(int priority, program_t program, uint64_t argc, char *argv[]){
    return userspaceCreateProcess(priority, program, argc, argv);
}

static uint64_t s_create_process_foreground(int priority, program_t program, uint64_t argc, char *argv[]){
    return userspaceCreateProcessForeground(priority, program, argc, argv);
}

static void s_create_process_set_fd(int * fd_ids, int fd_count){
    userspaceSetFD(fd_ids, fd_count);
}

static uint64_t s_kill_process(uint64_t pid){
    return kill_process(pid);
}

static char * s_list_processes(){
    return list_processes();
}

static uint64_t s_getPID(){
    return get_PID();
}

static void s_yield(){
    yield();
}

static void s_block_process(uint64_t pid){
    block_process_pid(pid);
}

static void s_unblock_process(uint64_t pid){
    unblock_process(pid);
}

static char * sys_mem_state(){
    return mem_state();
}

static char sys_read_fd(uint64_t fdIndex){
    return readFD(fdIndex);
}

static char sys_write_fd(uint64_t fdIndex, char data){
    return writeFD(fdIndex, data);
}

static uint64_t sys_open_fd(uint64_t fd_id){
    return openFD(fd_id);
}

static int sys_close_fd(uint64_t fd_id){
    return closeFD(fd_id);
}

static uint64_t sys_pipe_create(){
    return pipeCreate();
}

static uint64_t sys_set_priority(uint64_t pid, uint64_t priority){
    return setPriority(pid, priority);
}







/* Arreglo de punteros a funciones (syscalls) */

static uint64_t (*syscalls[])(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t) = {
    /* RAX: número de syscall */
    (void *)sys_read,               // 0
    (void *)sys_write,              // 1
    (void *)sys_clear,              // 2
    (void *)sys_getHours,           // 3
    (void *)sys_getMinutes,         // 4
    (void *)sys_getSeconds,         // 5
    (void *)sys_getScrHeight,       // 6
    (void *)sys_getScrWidth,        // 7
    (void *)sys_drawRectangle,      // 8
    (void *)sys_wait,               // 9
    (void *)sys_registerInfo,       // 10
    (void *)sys_printmem,           // 11
    (void *)sys_pixelPlus,          // 12
    (void *)sys_pixelMinus,         // 13
    (void *)sys_playSpeaker,        // 14
    (void *)sys_stopSpeaker,        // 15
    (void *)sys_drawCursor,         // 16
    (void *)sys_writeColor,         // 17
    (void *)sys_mem_alloc,          // 18
    (void *)sys_mem_free,           // 19
    (void *)sys_mem_init,           // 20
    (void *)s_create_process,   // 21
    (void *)s_kill_process,     // 22
    (void *)s_getPID,           // 23
    (void *)s_list_processes,   // 24
    (void *)s_block_process,    // 25
    (void *)s_unblock_process,  // 26
    (void *)s_yield,            // 27
    (void *)sys_sem_open,           // 28
    (void *)sys_sem_close,          // 29
    (void *)sys_sem_wait,           // 30
    (void *)sys_sem_post,            // 31
    (void *)sys_wait_pid,         // 32
    (void *)sys_read_fd,            // 33
    (void *)sys_write_fd,           // 34
    (void *)sys_open_fd,            // 35
    (void *)sys_close_fd,        // 36
    (void *)s_create_process_foreground, // 37
    (void *)s_create_process_set_fd, // 38
    (void *)sys_pipe_create,        // 39
    (void *)sys_set_priority,       // 40
    (void *)sys_mem_state,          // 41
};

uint64_t syscall_dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t rax) {
    if (syscalls[rax] != 0){
        return syscalls[rax](rdi, rsi, rdx, r10, r8);
    }

    return 0;
}

// uint64_t syscall_dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t rax)
// {
//     uint8_t r, g, b;
//     Color color;
//     switch (rax)
//     {
//     case 0:
//         return sys_read(rdi, (char *)rsi);
//     case 1:
//         return sys_write(rdi, (char)rsi);
//     case 2:
//         return sys_clear();
//     case 3:
//         return sys_getHours();
//     case 4:
//         return sys_getMinutes();
//     case 5:
//         return sys_getSeconds();
//     case 6:
//         return sys_getScrHeight();
//     case 7:
//         return sys_getScrWidth();
//     case 8:
//         r = (r8 >> 16) & 0xFF;
//         g = (r8 >> 8) & 0xFF;
//         b = r8 & 0xFF;
//         color.r = r;
//         color.g = g;
//         color.b = b;
//         sys_drawRectangle(rdi, rsi, rdx, r10, color);
//         return 1;
//     case 9:
//         sys_wait(rdi);
//         return 1;
//     case 10:
//         return sys_registerInfo((uint64_t *)rdi);
//     case 11:
//         return sys_printmem((uint64_t *)rdi);
//     case 12:
//         return sys_pixelPlus();
//     case 13:
//         return sys_pixelMinus();
//     case 14:
//         return sys_playSpeaker((uint32_t)rdi, rsi);
//     case 15:
//         return sys_stopSpeaker();
//     case 16:
//         return sys_drawCursor();
//     case 17:
//         r = (rdx >> 16) & 0xFF;
//         g = (rdx >> 8) & 0xFF;
//         b = rdx & 0xFF;
//         color.r = r;
//         color.g = g;
//         color.b = b;
//         return sys_writeColor(rdi, (char)rsi, color);
//     case 18: 
//         return (uint64_t)sys_mem_init((void*)rdi, rsi);
//     case 19: 
//         return (uint64_t)sys_mem_alloc(rdi);
//     case 20:
//         sys_mem_free((void*)rdi);
//     case 21:
//         return s_create_process(rdi, (program_t)rsi, rdx, (char **)r10);
//     case 22:
//         s_kill_process(rdi);
//         return 0;
//     case 23:
//         s_list_processes((char *)rdi);
//         return 0;
//     case 24:
//         return s_getPID();
//     case 25:
//         s_yield();
//         return 0;
//     case 26:
//         s_block_process(rdi);
//         return 0;
//     case 27:
//         s_unblock_process(rdi);
//         return 0;
//     case 28:
//         return (uint64_t)sys_sem_open((char *)rdi, rsi);
//     case 29:
//         sys_sem_close((char *)rdi);
//         return 0;
//     case 30:
//         sys_sem_wait((char *)rdi);
//         return 0;
//     case 31:
//         sys_sem_post((char *)rdi);
//         return 0;
//     default:
//         return 0;
//     }
// }