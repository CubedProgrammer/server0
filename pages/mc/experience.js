lvlElem = document.getElementById('level')
expElem = document.getElementById('experience')
function calcExp()
{
    var lvl = parseInt(lvlElem.value)
    if(lvl < 17)
        lvl *= lvl + 6
    else if(lvl < 32)
        lvl = (lvl * lvl * 5 - lvl * 81 + 720) / 2
    else
        lvl = (lvl * lvl * 9 - lvl * 325 + 4440) / 2
    expElem.value = lvl
}
