/**************************************
**               HEADER
***************************************/

#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>

#define ERR_BAD_ARGUMENTS 	"Error: argument"
#define ERR_FILE_CORRUPTED	"Error: Operation file corrupted"

#define HEAD "%d %d %c\n"
#define OPER "%c %f %f %f %c\n"

enum      e_pos
{
    OUTSIDE = 0,
    INSIDE = 1,
    BORDER = 2
};

typedef struct  s_prm
{
    char id;
    float x;
    float y;
    float r;
    char cl;
}               t_prm;

typedef struct s_field
{
    int w;
    int h;
    char bc;
}               t_field;

typedef struct  s_all
{
    int res;
    t_field f;
    t_prm p;
    FILE *file;
    char *buf;
}               t_all;

/**************************************
**               UTILS
***************************************/

int ft_strlen(char *str)
{
    int i;

    i = 0;
    while (str && str[i] != '\0')
        i++;
    return (i);
}

int     put_error(char *str)
{
    write(1, str, ft_strlen(str));
    write(1, "\n", 1);
    return (1);
}

/**************************************
**               ALGO
***************************************/

int  init(t_all *all)
{
    all->buf = malloc(all->f.w * all->f.h);
    if (all->buf == NULL)
        return (1);
    return (0);
}

int     in_circle(t_all *all, float x, float y)
{
    float dist = sqrtf((x - all->p.x) * (x - all->p.x) + (y - all->p.y) * (y - all->p.y));
    if (dist > all->p.r)
    {
        return (OUTSIDE);
    }
    else if (((dist - all->p.r) * (dist - all->p.r)) < 1.0)
    {
        return (BORDER);
    }
    return (INSIDE);
}

void    write_background(t_all *all)
{
    int i;
    int j;
    int res;

    i = 0;
    while (i < all->f.h)
    {
        j = 0;
        while (j < all->f.w)
        {
            all->buf[i * all->f.w + j] = all->f.bc;
            j++;
        }
        i++;
    }
}

void    write_circle(t_all *all)
{
    int i;
    int j;
    int pos;

    i = 0;
    while (i < all->f.h)
    {
        j = 0;
        while (j < all->f.w)
        {
            pos = in_circle(all, j, i);
            if ((all->p.id == 'c' && pos == BORDER) || (all->p.id == 'C' && pos != OUTSIDE))
                all->buf[i * all->f.w + j] = all->p.cl;
            j++;
        }
        i++;
    }
}

int     parse_head(t_all *all)
{
    all->res = fscanf(all->file, HEAD, &all->f.w, &all->f.h, &all->f.bc);
    if (all->f.w <= 0 || all->f.w > 300 || all->f.h <= 0 || all->f.h > 300)
        return (1);
    return (0);
}

int     parse_prm(t_all *all)
{
    all->res = fscanf(all->file, OPER, &all->p.id, &all->p.x, &all->p.y, &all->p.r, &all->p.cl);
    if (all->p.r <= 0.0)
        return (1);
    return (0);
}

int     try_open(t_all *all, char *filename)
{
    all->file = fopen(filename, "r");
    if (all->file == NULL)
        return (1);
    return (0);
}

int     main_cycle(t_all *all)
{
    if (parse_head(all) || all->res != 3 || init(all))
        return (1);
    write_background(all);
    all->res = 5;
    while (all->res == 5)
    {
        if (parse_prm(all))
            return (1);
        if (all->res == 5)
            write_circle(all);
    }
    if (all->res != -1)
        return (1);
    return (0);
}

void    draw_all(t_all *all)
{
    int i;
    
    i = 0;
    while (i < all->f.h)
    {
        write(1, all->buf + all->f.w * i, all->f.w);
        write(1, "\n", 1);
        i++;
    }
}

int main(int argc, char **argv)
{
    t_all all;

    if (argc != 2)
        return (put_error(ERR_BAD_ARGUMENTS));
    else if (try_open(&all, argv[1]) || main_cycle(&all))
        return (put_error(ERR_FILE_CORRUPTED));
    draw_all(&all);
    fclose(all.file);
    free(all.buf);
}