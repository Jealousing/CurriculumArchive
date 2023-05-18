using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

/*
 다른 script에서 가져다 쓰기 위한 script
 */

//아이템 코드
public enum E_ItemCode
{ 
    Air=0,
    Wood,
    Stick,
    String,
    Carrot,
    WoodPlank,

    FishingRod,
    WoodenAxe,
    CarrotOnAStick,
    DiamondHelmet,
    DiamondChestplate,
    DiamondLeggings,
    DiamondBoots,

}

//아이템 타입
public enum E_ItemType
{
    Normal=0,
    NonOverlap,
    Helmet,
    Chestplate,
    Leggings,
    Boots

}


//2차원 배열을 인스팩터에 띄우기위해
[Serializable]
public class ItemData_Array
{
    [SerializeField]
    public ItemData[] array;
}

[Serializable]
public class ItemSlotControls_Array
{
    [SerializeField]
    public ItemSlotControl[] array;
}

