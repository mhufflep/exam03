/**************************************
**               HEADER
***************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define ERR_BAD_ARGUMENTS 	"Error: argument"
#define ERR_FILE_CORRUPTED	"Error: Operation file corrupted"

#define HEAD "%d %d %c\n"
#define OPER "%c %f %f %f %f %c\n"

enum      e_pos
{
    OUTSIDE = 0,
    INSIDE = 1,
    BORDER = 2
};

typedef struct  s_field
{
    int         w;
    int         h;
    char        bc;
}               t_field;

typedef struct  s_prm
{
    char        id;
    float       x;
    float       y;
    float       w;
    float       h;
    char        cl;
}               t_prm;

typedef struct  s_all
{
    FILE        *file;
    t_field     f;
    t_prm       p;
    int         res;
    char        *buf;
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
        return (OUTSIDE);
    }
    if (((x - all->p.x < 1.0) || ((all->p.x + all->p.w) - x < 1.0)) || ((y - all->p.y < 1.0) || ((all->p.y + all->p.h) - y < 1.0)))
    {
        return (BORDER);
    }
    return (INSIDE);
}

void     write_rectangle(t_all *all)
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
            pos = is_rect(all, i, j);
            if ((all->p.id == 'R' && pos != OUTSIDE) || (all->p.id == 'r' && pos == BORDER))
                all->buf[all->f.w * i + j] = all->p.cl;
            j++;
        }
        i++;
    }
}

int     read_conf_line(t_all *all, t_prm *p)
{
    all->res = fscanf(all->file, OPER, &(p->id), &(p->x), &(p->y), &(p->w), &(p->h), &(p->cl));
    if (p->w <= 0.0 || p->h <= 0.0 || (p->id != 'r' && p->id != 'R'))
        return (1);
    return (0);
}

int read_field_line(t_all *all, t_field *f)
{
	all->res = fscanf(all->file, HEAD, &f->w, &f->h, &f->bc);
	if (all->res != 3 || f->w <= 0 || f->w > 300 || f->h <= 0 || f->h > 300)
		return (1);
	if (all->res == -1)
		return (1);
	return (0);
}

int allocate_buffer(t_all *all)
{
    all->buf = malloc(sizeof(char) * all->f.w * all->f.h);
    if (all->buf == NULL)
        return (1);
    return (0);
}

int read_cycle(t_all *all)
{
    read_field_line(all, &all->f);
    if (allocate_buffer(all))
        return (1);
    write_background(all);
    read_conf_line(all, &all->p);
    while (all->res == 6)
    {
        write_rectangle(all);
        if (read_conf_line(all, &all->p))
            return (1);
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

void clean_all(t_all *all)
{
    free(all->buf);
    fclose(all->file);
}

int main(int argc, char **argv)
{
    t_all all;

    if (argc != 2)
        return (put_error(ERR_BAD_ARGUMENTS));
    all.file = fopen(argv[1], "r");
    if (!all.file)
        return (put_error(ERR_FILE_CORRUPTED));
    if (read_cycle(&all))
	{
        clean_all(&all);
        return (put_error(ERR_FILE_CORRUPTED));
	}
    draw_all(&all);
    clean_all(&all);
    return (0);
}