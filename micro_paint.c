
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define HEAD "%d %d %c\n"
#define OPER "%c %f %f %f %f %c\n"

typedef struct  s_field
{
    int         w;
    int         h;
    char        bc;
}               t_field;

typedef struct  s_prm
{
    char    id;
    float   x;
    float   y;
    float   w;
    float   h;
    char    cl;
}               t_prm;

typedef struct  s_all
{
    FILE        *file;
    t_field     f;
    t_prm       p;
    int         res;
    char        *buf;
}               t_all;

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

void    write_background(t_all *all)
{
    int i;
    int j;

    i = 0;
    while (i < all->f.h)
    {
        j = 0;
        while (j < all->f.w)
        {
            all->buf[all->f.w * i + j] = all->f.bc;
            j++;
        }
        i++;
    }
}

int     is_rect(t_all *all, float y, float x)
{
    if ((y < all->p.y) || (y > all->p.y + all->p.h) || (x < all->p.x) || (x > all->p.x + all->p.w))
    {
        return (0);
    }
    if (((x - all->p.x < 1.0) || ((all->p.x + all->p.w) - x < 1.0)) || ((y - all->p.y < 1.0) || ((all->p.y + all->p.h) - y < 1.0)))
    {
        return (2);
    }
    return (1);
}

void     write_rectangle(t_all *all)
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
            res = is_rect(all, i, j);
            if ((all->p.id == 'R' && res > 0) || (all->p.id == 'r' && res == 2))
                all->buf[all->f.w * i + j] = all->p.cl;
            j++;
        }
        i++;
    }
}

int     read_head_line(t_all *all, t_field *f)
{   
    int res;

    res = fscanf(all->file, HEAD, &f->w, &f->h, &f->bc);
    if (f->w <= 0 || f->w > 300 || f->h <= 0 || f->h > 300)
        return (1);
    all->res = res;
    return (0);
}

int     read_conf_line(t_all *all, t_prm *p)
{   
    int res;

    res = fscanf(all->file, OPER, &(p->id), &(p->x), &(p->y), &(p->w), &(p->h), &(p->cl));
    if (p->w <= 0.0 || p->h <= 0.0)
        return (1);
    all->res = res;
    return (0);
}

int initialize(t_all *all)
{
    all->buf = malloc(all->f.w * all->f.h);
    if (all->buf == NULL)
        return (1);
    return (0);
}

int read_cycle(t_all *all)
{
    read_head_line(all, &all->f);
    if (all->res != 3 || initialize(all))
        return (1);
    write_background(all);

    all->res = 6;
    while (all->res == 6)
    {
        if (read_conf_line(all, &all->p))
            return (1);
        if (all->res == 6)
            write_rectangle(all);
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
        write(1, all->buf + i * all->f.w, all->f.w);
        write(1, "\n", 1);
        i++;
    }
}

int main(int argc, char **argv)
{
    t_all all;

    if (argc != 2)
    {
        return (put_error("Error: argument"));
    }
    else
    {
        all.file = fopen(argv[1], "r");
        if (!all.file || read_cycle(&all))
            return (put_error("Error: Operation file corrupted"));
    }
    draw_all(&all);
    printf("%c\n", all.buf[all.f.w * all.f.h]);
    free(all.buf);
    fclose(all.file);
    return (0);
}