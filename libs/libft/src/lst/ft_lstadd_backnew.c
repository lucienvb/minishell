#include <libft.h>

/**
 * @brief Takes ’content’ and adds it to the end of the list.
 *
 * @param lst The address of a pointer to the first link of a list.
 * @param new The address of a pointer to the content to be
 * allocated and added to the list.
 */
t_list	*ft_lstadd_backnew(t_list **lst, void *content)
{
	t_list	*new_node;

	new_node = ft_lstnew(content);
	if (!new_node)
		return (NULL);
	ft_lstadd_back(lst, new_node);
	return (new_node);
}
