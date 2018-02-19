#ifndef SIMPLE_XML_H_
#define SIMPLE_XML_H_

#define NODE_START    (1 << 0)
#define NODE_END      (1 << 1)
#define NODE_CLOSE    (1 << 2)
#define NODE_NAME     (1 << 3)
#define EQUALS        (1 << 4)
#define COMMENT_START (1 << 5)
#define COMMENT_END   (1 << 6)
#define CDATA_START   (1 << 7)
#define CDATA_END     (1 << 8)
#define QUOTED        (1 << 9)
#define APOSED        (1 << 10)
#define PROLOG        (1 << 11)
#define NODE_MID      (1 << 12)
#define ATT_NAME      (1 << 13)

#endif // SIMPLE_XML_H_
