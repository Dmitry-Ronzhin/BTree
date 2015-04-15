﻿#include "stdafx.h"
#include "mydb_cache.h"

/*   LRU cache с поддержкой write-back позволяет отложенно записывать изменения
 *   B-tree на диск. Для реализации кэша необходимо реализовать
 *   журнал (Write Ahead Log), в котором логируются все изменения
 *   блоков дерева.
 *   
 *   Перед изменением блока, запись попадает в журнал. В дальнейшем,
 *   блок может быть «вытеснен» из кэша на диск.
 *   
 *   Предполагается, что кэширование «подключается» к текущему дереву на уровне API
 *   block_read () / block_write (). 
 *
 *   То есть, функция block_read (), к примеру, перед тем как прочитать блок с диска,
 *   осуществляет поиск этого блока в кэше. Если блок не найден, он читается из журнала в кэш.
 *   Если кэш заполнен, вытеснение и поиск свободного блока осуществляется
 *   в соответствии с алгоритмом LRU.
 *
 *   block_write () обновляет блок в кэше с новым значением, и добавляет
 *   его в список «грязных» блоков. Грязный блок должен быть записан
 *   на диск перед вытеснением из кэша.
 *   
 *   В случае crash'a приложения, db_open () обязана уметь восстанавливать
 *   последнее состояние B-tree по журналу.
 */