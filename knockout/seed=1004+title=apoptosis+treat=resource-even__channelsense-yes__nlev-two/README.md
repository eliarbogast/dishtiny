# Source Update

`2**20`

# ID Key

1. wild type
2. knockout
  * `DoApoptosisComplete`

# Knockout Prep

```
for f in *; do cp $f id=1+$f; done
for f in c*; do mv $f id=2+$f; done
sed -i -- 's/DoApoptosisComplete/Nop/g' id\=2+component\=program*
```