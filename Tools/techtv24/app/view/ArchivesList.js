/*
 * File: app/view/ArchivesList.js
 *
 * This file was generated by Sencha Architect version 2.0.0.
 * http://www.sencha.com/products/architect/
 *
 * This file requires use of the Sencha Touch 2.0.x library, under independent license.
 * License of Sencha Architect does not include license for Sencha Touch 2.0.x. For more
 * details see http://www.sencha.com/license or contact license@sencha.com.
 *
 * This file will be auto-generated each and everytime you save your project.
 *
 * Do NOT hand edit this file.
 */

Ext.define('app.view.ArchivesList', {
    extend: 'Ext.dataview.List',
    alias: 'widget.archiveslist',

    config: {
        scrollable: 'vertical',
        loadingText: 'Loading Archives...',
        store: 'ArchivesStore',
        onItemDisclosure: true,
        itemTpl: [
            '<section class="channelListItem">',
            '<img src="data:image/jpeg;base64,{thumbnail}">',
            '<h1>{title}</h1>',
            '</section>'
        ],
        listeners: [
            {
                fn: 'onListDisclose',
                event: 'disclose'
            }
        ]
    },

    onListDisclose: function(list, record, target, index, e, options) {
        list.up('navigationview').push({
            xtype: 'archivedetails',
            data: record.data,
            title: record.get('title')
        });
    }

});