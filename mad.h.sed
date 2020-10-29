/^\/\*$/{
N
s/ \* mad - /&/
t copy
b next
: copy
g
n
s|^ \* \$\(Id: .*\) \$$|/* \1 */|p
/^ \*\/$/d
b copy
}
/^# *include "/d
: next
p
