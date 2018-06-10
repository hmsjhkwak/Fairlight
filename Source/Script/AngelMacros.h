#ifndef SCRIPT_ANGEL_MACROS_H
#define SCRIPT_ANGEL_MACROS_H

#define SCRIPT_ASSERT(statement, message) \
    do { \
        if (!(statement)) { \
            asGetActiveContext()->SetException(message); return; \
        } \
    } while(0)

#define SCRIPT_ASSERT_RETVAL(statement, message, retval) \
    do { \
        if (!(statement)) { \
            asGetActiveContext()->SetException(message); return retval; \
        } \
    } while(0)

#define SCRIPT_ASSERT_ALIVE(en) SCRIPT_ASSERT(AngelState::getCurrent()->getEntityManager()->alive(en), "Entity is not alive.")
#define SCRIPT_ASSERT_ALIVE_RETVAL(en, retval) SCRIPT_ASSERT_RETVAL(AngelState::getCurrent()->getEntityManager()->alive(en), "Entity is not alive.", retval)

#endif
