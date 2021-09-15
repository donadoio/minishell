#include "../../includes/utils.h"

int	ft_isalnum_str(char *str)
{
    int i;

    i = 0;
    if (str == NULL)
        return (-1);
    while (str[i] != '\0')
    {
        if (ft_isalpha(str[i]) == 1)
            i++;
        else if (ft_isdigit(str[i]) == 1)
            i++;
        else if (str[i] == '_')
            i++;
        else
            return (0);
    }
    return (1);
}
