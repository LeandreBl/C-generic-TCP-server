#ifndef INTERNAL_H_
# define INTERNAL_H_

# include "lserver.h"
# include "lclient.h"

void _lserver_lclient_destructor(void *ptr) __THROW __nonnull((1));

#endif /* !INTERNAL_H_ */