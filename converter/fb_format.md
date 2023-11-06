# Furball converted C file format

Everything is little-endian.

## Pattern Row

### Pattern flags

> Note that flag is stored as `short` due to the alignment.

max effects count range: [0..8]

<table>
  <tr>
    <td>7</td>
    <td>6</td>
    <td>5</td>
    <td>4</td>
    <td>3</td>
    <td>2</td>
    <td>1</td>
    <td>0</td>
  </tr>
  <tr>
    <td></td>
    <td>vol?</td>
    <td>note?</td>
    <td>inst?</td>
    <td colspan="4">max effects count</td>
  </tr>
</table>
