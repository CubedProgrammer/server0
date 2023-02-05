lvlElem = document.getElementById('level')
expElem = document.getElementById('experience')
tableElem = document.getElementById('chart')
lvlStart = 1
tableTxtNodes = []
document.addEventListener('keypress', keyScrollTable)
for(var i = 1; i <= 24; ++i)
{
    var trnode = document.createElement('tr')
    var lvltd = document.createElement('td')
    var exptd = document.createElement('td')
    var lvltxt = i.toString(10)
    var exptxt = lvlToExp(i).toString(10)
    var lvltn = document.createTextNode(lvltxt)
    var exptn = document.createTextNode(exptxt)
    lvltd.appendChild(lvltn)
    exptd.appendChild(exptn)
    trnode.appendChild(lvltd)
    trnode.appendChild(exptd)
    tableElem.appendChild(trnode)
    tableTxtNodes.push([lvltd, exptd])
}
function updateTable()
{
    var start = lvlStart
    for(var r of tableTxtNodes)
    {
        r[0].innerText = start.toString(10)
        r[1].innerText = lvlToExp(start).toString(10)
        ++start
    }
}
function keyScrollTable(e)
{
    k = e.keyCode
    if(k === 115)
        ++lvlStart
    else if(k === 119 && lvlstart > 0)
        --lvlstart
    else if(k === 97)
        lvlStart = Math.max(0, lvlStart - 24)
    else if(k === 100)
        lvlStart += 24
    updateTable()
}
function lvlToExp(lvl)
{
    if(lvl < 17)
        lvl *= lvl + 6
    else if(lvl < 32)
        lvl = (lvl * lvl * 5 - lvl * 81 + 720) / 2
    else
        lvl = (lvl * lvl * 9 - lvl * 325 + 4440) / 2
    return lvl
}
function expToLvl(exp)
{
    if(exp < 394)
        exp = Math.min(16, Math.floor(Math.sqrt(9 + exp) - 3))
    else if(exp < 1628)
        exp = Math.min(31, Math.floor(Math.sqrt(40 * exp - 7839) / 10 + 8.1))
    else
        exp = Math.floor((Math.sqrt(72 * exp - 54359) + 325) / 18)
    return exp
}
function calcLvl()
{
    var exp = parseInt(expElem.value)
    var lvl = expToLvl(exp)
    exp -= lvlToExp(lvl)
    expElem.value = exp
    lvlElem.value = lvl
}
function calcExp()
{
    var lvl = parseInt(lvlElem.value)
    expElem.value = lvlToExp(lvl)
}
