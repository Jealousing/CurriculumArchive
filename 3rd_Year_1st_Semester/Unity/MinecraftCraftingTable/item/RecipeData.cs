using System.Collections;
using System.Collections.Generic;
using UnityEngine;

//레시피 스크립트

[CreateAssetMenu(fileName = "New RecipeData", menuName = "RecipeData")]
public class RecipeData : ScriptableObject //스크립트 오브젝트
{
    [SerializeField]
    public ItemData_Array[] RecipeArray = new ItemData_Array[3]; //3x3 2차원배열
    public string GetName;       //이름 인스펙터에서 확인용
    public ItemData GetItem;    //조합결과
    public int GetNum;               //조합결과에 나오는 아이템 갯수
}

