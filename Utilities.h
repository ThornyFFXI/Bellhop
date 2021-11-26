#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#define RBUFP(p, pos) (((uint8_t*)(p)) + (pos))
#define Read8(p, pos) (*(uint8_t*)RBUFP((p), (pos)))
#define Read16(p, pos) (*(uint16_t*)RBUFP((p), (pos)))
#define Read32(p, pos) (*(uint32_t*)RBUFP((p), (pos)))
#define Read64(p, pos) (*(uint64_t*)RBUFP((p), (pos)))
#define ReadFloat(p, pos) (*(float_t*)RBUFP((p), (pos)))

#define WBUFP(p, pos) (((uint8_t*)(p)) + (pos))
#define Write8(p, pos) (*(uint8_t*)WBUFP((p), (pos)))
#define Write16(p, pos) (*(uint16_t*)WBUFP((p), (pos)))
#define Write32(p, pos) (*(uint32_t*)WBUFP((p), (pos)))
#define Write64(p, pos) (*(uint64_t*)WBUFP((p), (pos)))
#define WriteFloat(p, pos) (*(float_t*)WBUFP((p), (pos)))

#define CheckArg(a, b) (argcount > a) && (_stricmp(args[a].c_str(), b) == 0)

#endif _UTILITIES_H_