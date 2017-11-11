//
//  MenuItemProperties.cpp
//  libraries/script-engine/src
//
//  Created by Brad Hefta-Gaub on 1/28/14.
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include <QDebug>
#include <RegisteredMetaTypes.h>
#include "MenuItemProperties.h"



MenuItemProperties::MenuItemProperties(const QString& menuName, const QString& menuItemName,
                        const QString& shortcutKey, bool checkable, bool checked, bool separator) :
    menuName(menuName),
    menuItemName(menuItemName),
    shortcutKey(shortcutKey),
    shortcutKeyEvent(),
    shortcutKeySequence(shortcutKey),
    isCheckable(checkable),
    isChecked(checked),
    isSeparator(separator)
{
}

MenuItemProperties::MenuItemProperties(const QString& menuName, const QString& menuItemName,
                        const KeyEvent& shortcutKeyEvent, bool checkable, bool checked, bool separator) :
    menuName(menuName),
    menuItemName(menuItemName),
    shortcutKeyEvent(shortcutKeyEvent),
    shortcutKeySequence(shortcutKeyEvent),
    isCheckable(checkable),
    isChecked(checked),
    isSeparator(separator)
{
}

void registerMenuItemProperties(QScriptEngine* engine) {
    qScriptRegisterMetaType(engine, menuItemPropertiesToScriptValue, menuItemPropertiesFromScriptValue);
}

QScriptValue menuItemPropertiesToScriptValue(QScriptEngine* engine, const MenuItemProperties& properties) {
    QScriptValue obj = engine->newObject();
    // not supported
    return obj;
}

/**jsdoc
 * A set of properties that can be passed to {@link Menu.addMenuItem} to create a new menu item.
 *
 * If none of <code>position</code>, <code>beforeItem</code>, <code>afterItem</code>, or <code>grouping</code> are specified, 
 * the menu item will be placed at the end of the menu.
 *
 * @typedef {Object} Menu.MenuItemProperties
 * @property {String} menuName Name of the menu.
 * @property {String} menuItemName Name of the menu item.
 * @property {Boolean} [isCheckable=false] Whether or not the menu item is checkable.
 * @property {Boolean} [isChecked=false] Whether or not the menu item is checked.
 * @property {Boolean} [isSeparator=false] Whether or not the menu item is a separator.
 * @property {String} [shortcutKey] A shortcut key that triggers the menu item.
 * @property {KeyEvent} [shortcutKeyEvent] A {@link KeyEvent} that specifies a key that triggers the menu item.
 * @property {Number} [position] The position to place the new menu item. An integer number with <code>0</code> being the first
 *     menu item.
 * @property {String} [beforeItem] The name of the menu item to place this menu item before.
 * @property {String} [afterItem] The name of the menu item to place this menu item after.
 * @property {String} [grouping] The name of grouping to add this menu item to.
 */
void menuItemPropertiesFromScriptValue(const QScriptValue& object, MenuItemProperties& properties) {
    properties.menuName = object.property("menuName").toVariant().toString();
    properties.menuItemName = object.property("menuItemName").toVariant().toString();
    properties.isCheckable = object.property("isCheckable").toVariant().toBool();
    properties.isChecked = object.property("isChecked").toVariant().toBool();
    properties.isSeparator = object.property("isSeparator").toVariant().toBool();

    // handle the shortcut key options in order...
    QScriptValue shortcutKeyValue = object.property("shortcutKey");
    if (shortcutKeyValue.isValid()) {
        properties.shortcutKey = shortcutKeyValue.toVariant().toString();
        properties.shortcutKeySequence = properties.shortcutKey;
    } else {
        QScriptValue shortcutKeyEventValue = object.property("shortcutKeyEvent");
        if (shortcutKeyEventValue.isValid()) {
            KeyEvent::fromScriptValue(shortcutKeyEventValue, properties.shortcutKeyEvent);
            properties.shortcutKeySequence = properties.shortcutKeyEvent;
        }
    }

    if (object.property("position").isValid()) {
        properties.position = object.property("position").toVariant().toInt();
    }
    properties.beforeItem = object.property("beforeItem").toVariant().toString();
    properties.afterItem = object.property("afterItem").toVariant().toString();
    properties.grouping = object.property("grouping").toVariant().toString();
}


