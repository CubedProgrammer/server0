lvlElem = document.getElementById('level')
expElem = document.getElementById('experience')
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
