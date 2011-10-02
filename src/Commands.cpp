/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include "Commands.h"

#include <KLocale>

#include "BackgroundImage.h"
#include "Document.h"
#include "Editor.h"
#include "Floss.h"
#include "FlossScheme.h"
#include "MainWindow.h"
#include "Palette.h"
#include "Preview.h"
#include "SchemeManager.h"


AddStitchCommand::AddStitchCommand(Document *document, int layer, const QPoint &location, Stitch::Type type, int colorIndex)
	:	QUndoCommand(),
		m_document(document),
		m_layer(layer),
		m_cell(location),
		m_type(type),
		m_colorIndex(colorIndex),
		m_original(0)
{
}


AddStitchCommand::~AddStitchCommand()
{
	delete m_original;
}


void AddStitchCommand::redo()
{
	m_original = m_document->stitchData().stitchQueueAt(m_layer, m_cell);
	if (m_original)
		m_document->stitchData().replaceStitchQueueAt(m_layer, m_cell, new StitchQueue(m_original));
	m_document->stitchData().addStitch(m_layer, m_cell, m_type, m_colorIndex);
}


void AddStitchCommand::undo()
{
	delete m_document->stitchData().replaceStitchQueueAt(m_layer, m_cell, m_original);
	m_original = 0;
}


DeleteStitchCommand::DeleteStitchCommand(Document *document, int layer, const QPoint &cell, Stitch::Type type, int colorIndex)
	:	QUndoCommand(),
		m_document(document),
		m_layer(layer),
		m_cell(cell),
		m_type(type),
		m_colorIndex(colorIndex),
		m_original(0)
{
}


DeleteStitchCommand::~DeleteStitchCommand()
{
	delete m_original;
}


void DeleteStitchCommand::redo()
{
	m_original = m_document->stitchData().stitchQueueAt(m_layer, m_cell);
	if (m_original)
		m_original = m_document->stitchData().replaceStitchQueueAt(m_layer, m_cell, new StitchQueue(m_original));
	m_document->stitchData().deleteStitch(m_layer, m_cell, m_type, m_colorIndex);
}


void DeleteStitchCommand::undo()
{
	delete m_document->stitchData().replaceStitchQueueAt(m_layer, m_cell, m_original);
	m_original = 0;
}


AddBackstitchCommand::AddBackstitchCommand(Document *document, int layer, const QPoint &start, const QPoint &end, int colorIndex)
	:	QUndoCommand(i18n("Add Backstitch")),
		m_document(document),
		m_layer(layer),
		m_start(start),
		m_end(end),
		m_colorIndex(colorIndex)
{
}


AddBackstitchCommand::~AddBackstitchCommand()
{
}


void AddBackstitchCommand::redo()
{
	m_document->stitchData().addBackstitch(m_layer, m_start, m_end, m_colorIndex);
}


void AddBackstitchCommand::undo()
{
	delete m_document->stitchData().takeBackstitch(m_layer, m_start, m_end, m_colorIndex);
}


DeleteBackstitchCommand::DeleteBackstitchCommand(Document *document, int layer, const QPoint &start, const QPoint &end, int colorIndex)
	:	QUndoCommand(i18n("Delete Backstitch")),
		m_document(document),
		m_layer(layer),
		m_start(start),
		m_end(end),
		m_colorIndex(colorIndex),
		m_backstitch(0)
{
}


DeleteBackstitchCommand::~DeleteBackstitchCommand()
{
	delete m_backstitch;
}


void DeleteBackstitchCommand::redo()
{
	if (m_backstitch)
		delete m_backstitch;
	m_backstitch = m_document->stitchData().takeBackstitch(m_layer, m_start, m_end, m_colorIndex);
}


void DeleteBackstitchCommand::undo()
{
	m_document->stitchData().addBackstitch(m_layer, m_backstitch->start, m_backstitch->end, m_backstitch->colorIndex);
}


AddKnotCommand::AddKnotCommand(Document *document, int layer, const QPoint &snap, int colorIndex)
	:	QUndoCommand(),
		m_document(document),
		m_layer(layer),
		m_snap(snap),
		m_colorIndex(colorIndex)
{
}


AddKnotCommand::~AddKnotCommand()
{
}


void AddKnotCommand::redo()
{
	m_document->stitchData().addFrenchKnot(m_layer, m_snap, m_colorIndex);
}


void AddKnotCommand::undo()
{
	delete m_document->stitchData().takeFrenchKnot(m_layer, m_snap, m_colorIndex);
}


DeleteKnotCommand::DeleteKnotCommand(Document *document, int layer, const QPoint &snap, int colorIndex)
	:	QUndoCommand(),
		m_document(document),
		m_layer(layer),
		m_snap(snap),
		m_colorIndex(colorIndex),
		m_knot(0)
{
}


DeleteKnotCommand::~DeleteKnotCommand()
{
	delete m_knot;
}


void DeleteKnotCommand::redo()
{
	m_knot = m_document->stitchData().takeFrenchKnot(m_layer, m_snap, m_colorIndex);
}


void DeleteKnotCommand::undo()
{
	m_document->stitchData().addFrenchKnot(m_layer, m_knot->position, m_knot->colorIndex);
}


SetPropertyCommand::SetPropertyCommand(Document *document, const QString &name, const QVariant &value)
	:	QUndoCommand(i18n("Set Property")),
		m_document(document),
		m_name(name),
		m_value(value)
{
}


SetPropertyCommand::~SetPropertyCommand()
{
	// nothing needs to be done here
}


void SetPropertyCommand::redo()
{
	m_oldValue = m_document->property(m_name);
	m_document->setProperty(m_name, m_value);
}


void SetPropertyCommand::undo()
{
	m_document->setProperty(m_name, m_oldValue);
}


AddBackgroundImageCommand::AddBackgroundImageCommand(Document *document, BackgroundImage *backgroundImage, MainWindow *mainWindow)
	:	QUndoCommand(i18n("Add Background Image")),
		m_document(document),
		m_backgroundImage(backgroundImage),
		m_mainWindow(mainWindow)
{
}


AddBackgroundImageCommand::~AddBackgroundImageCommand()
{
}


void AddBackgroundImageCommand::redo()
{
	m_document->backgroundImages().addBackgroundImage(m_backgroundImage);
	m_mainWindow->updateBackgroundImageActionLists();
}


void AddBackgroundImageCommand::undo()
{
	m_document->backgroundImages().removeBackgroundImage(m_backgroundImage);
	m_mainWindow->updateBackgroundImageActionLists();
}


FitBackgroundImageCommand::FitBackgroundImageCommand(Document *document, BackgroundImage *backgroundImage, const QRect &rect)
	:	QUndoCommand(i18n("Fit Background to Selection")),
		m_document(document),
		m_backgroundImage(backgroundImage),
		m_rect(rect)
{
}


FitBackgroundImageCommand::~FitBackgroundImageCommand()
{
}


void FitBackgroundImageCommand::redo()
{
	m_rect = m_document->backgroundImages().fitBackgroundImage(m_backgroundImage, m_rect);
}


void FitBackgroundImageCommand::undo()
{
	m_rect = m_document->backgroundImages().fitBackgroundImage(m_backgroundImage, m_rect);
}


ShowBackgroundImageCommand::ShowBackgroundImageCommand(Document *document, BackgroundImage *backgroundImage, bool visible)
	:	QUndoCommand(i18n("Show Background Image")),
		m_document(document),
		m_backgroundImage(backgroundImage),
		m_visible(visible)
{
}


ShowBackgroundImageCommand::~ShowBackgroundImageCommand()
{
}


void ShowBackgroundImageCommand::redo()
{
	m_visible = m_document->backgroundImages().showBackgroundImage(m_backgroundImage, m_visible);
}


void ShowBackgroundImageCommand::undo()
{
	m_visible = m_document->backgroundImages().showBackgroundImage(m_backgroundImage, m_visible);
}


RemoveBackgroundImageCommand::RemoveBackgroundImageCommand(Document *document, BackgroundImage *backgroundImage, MainWindow *mainWindow)
	:	QUndoCommand(i18n("Remove Background Image")),
		m_document(document),
		m_backgroundImage(backgroundImage),
		m_mainWindow(mainWindow)
{
}


RemoveBackgroundImageCommand::~RemoveBackgroundImageCommand()
{
	// delete m_backgroundImage;
	// m_backgroundImage may also be deleted by the document, potential for a crash or memory leak
}


void RemoveBackgroundImageCommand::redo()
{
	m_document->backgroundImages().removeBackgroundImage(m_backgroundImage);
	m_mainWindow->updateBackgroundImageActionLists();
}


void RemoveBackgroundImageCommand::undo()
{
	m_document->backgroundImages().addBackgroundImage(m_backgroundImage);
	m_mainWindow->updateBackgroundImageActionLists();
}


AddDocumentFlossCommand::AddDocumentFlossCommand(Document *document, int key, DocumentFloss *documentFloss)
	:	QUndoCommand(),
		m_document(document),
		m_key(key),
		m_documentFloss(documentFloss)
{
}


AddDocumentFlossCommand::~AddDocumentFlossCommand()
{
}


void AddDocumentFlossCommand::redo()
{
	m_document->documentPalette().add(m_key, m_documentFloss);
}


void AddDocumentFlossCommand::undo()
{
	m_document->documentPalette().remove(m_key);
}


RemoveDocumentFlossCommand::RemoveDocumentFlossCommand(Document *document, int key, DocumentFloss *documentFloss)
	:	QUndoCommand(),
		m_document(document),
		m_key(key),
		m_documentFloss(documentFloss)
{
}


RemoveDocumentFlossCommand::~RemoveDocumentFlossCommand()
{
	delete m_documentFloss;
}


void RemoveDocumentFlossCommand::redo()
{
	m_document->documentPalette().remove(m_key);
}


void RemoveDocumentFlossCommand::undo()
{
	m_document->documentPalette().add(m_key, m_documentFloss);
}


ReplaceDocumentFlossCommand::ReplaceDocumentFlossCommand(Document *document, int key, DocumentFloss *documentFloss)
	:	QUndoCommand(),
		m_document(document),
		m_key(key),
		m_documentFloss(documentFloss)
{
}


ReplaceDocumentFlossCommand::~ReplaceDocumentFlossCommand()
{
	delete m_documentFloss;
}


void ReplaceDocumentFlossCommand::redo()
{
	m_documentFloss = m_document->documentPalette().replace(m_key, m_documentFloss);
}


void ReplaceDocumentFlossCommand::undo()
{
	m_documentFloss = m_document->documentPalette().replace(m_key, m_documentFloss);
}


ResizeDocumentCommand::ResizeDocumentCommand(Document *document, int width, int height)
	:	QUndoCommand(),
		m_document(document),
		m_width(width),
		m_height(height)
{
}


ResizeDocumentCommand::~ResizeDocumentCommand()
{
}


void ResizeDocumentCommand::redo()
{
	m_originalWidth = m_document->stitchData().width();
	m_originalHeight = m_document->stitchData().height();
	QRect extents = m_document->stitchData().extents();
	int minx = std::min(extents.left(), m_width - extents.width());
	m_xOffset = minx - extents.left();
	int miny = std::min(extents.top(), m_height - extents.height());
	m_yOffset = miny - extents.top();
	m_document->stitchData().movePattern(m_xOffset, m_yOffset);
	m_document->stitchData().resize(m_width, m_height);
}


void ResizeDocumentCommand::undo()
{
	m_document->stitchData().movePattern(-m_xOffset, -m_yOffset);
	m_document->stitchData().resize(m_originalWidth, m_originalHeight);
}


#if 0
ChangeFlossColorCommand::ChangeFlossColorCommand(Document *document, DocumentFloss *documentFloss, Floss *floss)
	:	QUndoCommand(),
		m_document(document),
		m_documentFloss(documentFloss),
		m_floss(floss)
{
}


ChangeFlossColorCommand::~ChangeFlossColorCommand()
{
}


void ChangeFlossColorCommand::redo()
{
	Floss *floss = m_documentFloss->documentPalette().floss();
	m_documentFloss->documentPalette().setFloss(m_floss);
	m_floss = floss;
}


void ChangeFlossColorCommand::undo()
{
	Floss *floss = m_documentFloss->floss();
	m_documentFloss->documentPalette().setFloss(m_floss);
	m_floss = floss;
}
#endif


CropToPatternCommand::CropToPatternCommand(Document *document)
	:	QUndoCommand(i18n("Crop to Pattern")),
		m_document(document)
{
}


CropToPatternCommand::~CropToPatternCommand()
{
}


void CropToPatternCommand::redo()
{
	m_originalWidth = m_document->stitchData().width();
	m_originalHeight = m_document->stitchData().height();
	QRect extents = m_document->stitchData().extents();
	m_xOffset = -extents.left();
	m_yOffset = -extents.top();
	m_document->stitchData().movePattern(m_xOffset, m_yOffset);
	m_document->stitchData().resize(extents.width(), extents.height());
}


void CropToPatternCommand::undo()
{
	m_document->stitchData().movePattern(-m_xOffset, -m_yOffset);
	m_document->stitchData().resize(m_originalWidth, m_originalHeight);
}


ExtendPatternCommand::ExtendPatternCommand(Document *document, int top, int left, int right, int bottom)
	:	QUndoCommand(i18n("Extend Pattern")),
		m_document(document),
		m_top(top),
		m_left(left),
		m_right(right),
		m_bottom(bottom)
{
}


ExtendPatternCommand::~ExtendPatternCommand()
{
}


void ExtendPatternCommand::redo()
{
	StitchData &stitchData = m_document->stitchData();
	stitchData.resize(stitchData.width()+m_left+m_right, stitchData.height()+m_top+m_bottom);
	stitchData.movePattern(m_left, m_top);
}


void ExtendPatternCommand::undo()
{
	StitchData &stitchData = m_document->stitchData();
	stitchData.movePattern(-m_left, -m_top);
	stitchData.resize(stitchData.width()-m_left-m_right, stitchData.height()-m_top-m_bottom);
}


UpdateEditorCommand::UpdateEditorCommand(Editor *editor)
	:	QUndoCommand(),
		m_editor(editor)
{
}


UpdateEditorCommand::~UpdateEditorCommand()
{
}


void UpdateEditorCommand::redo()
{
	m_editor->update();
}


void UpdateEditorCommand::undo()
{
	m_editor->update();
}


UpdatePaletteCommand::UpdatePaletteCommand(Palette *palette)
	:	QUndoCommand(),
		m_palette(palette)
{
}


UpdatePaletteCommand::~UpdatePaletteCommand()
{
}


void UpdatePaletteCommand::redo()
{
	m_palette->update();
}


void UpdatePaletteCommand::undo()
{
	m_palette->update();
}


UpdatePreviewCommand::UpdatePreviewCommand(Preview *preview)
	:	QUndoCommand(),
		m_preview(preview)
{
}


UpdatePreviewCommand::~UpdatePreviewCommand()
{
}


void UpdatePreviewCommand::redo()
{
	m_preview->update();
}


void UpdatePreviewCommand::undo()
{
	m_preview->update();
}


ChangeSchemeCommand::ChangeSchemeCommand(Document *document, const QString &schemeName)
	:	QUndoCommand(i18n("Change floss scheme")),
		m_document(document),
		m_schemeName(schemeName)
{
}


ChangeSchemeCommand::~ChangeSchemeCommand()
{
}


void ChangeSchemeCommand::redo()
{
	if (m_flosses.count() == 0) // if m_flosses is empty, initialise it with converted flosses
	{
		QMapIterator<int, DocumentFloss *> flossIterator(m_document->documentPalette().flosses());
		FlossScheme *scheme = SchemeManager::scheme(m_schemeName);
		while (flossIterator.hasNext())
		{
			flossIterator.next();
			int key = flossIterator.key();
			DocumentFloss *documentFloss = flossIterator.value();
			Floss *floss = scheme->convert(documentFloss->flossColor());
			m_flosses[key] = new DocumentFloss(floss->name(), documentFloss->stitchSymbol(), documentFloss->backstitchSymbol(), documentFloss->stitchStrands(), documentFloss->backstitchStrands());
			m_flosses[key]->setFlossColor(floss->color());
		}
	}

	QMapIterator<int, DocumentFloss *> flossIterator(m_flosses);
	while (flossIterator.hasNext())
	{
		int key = flossIterator.key();
		m_flosses[key] = m_document->documentPalette().replace(key, m_flosses[key]);
	}

	QString schemeName = m_document->documentPalette().schemeName();
	m_document->documentPalette().setSchemeName(m_schemeName);
	m_schemeName = schemeName;
}


void ChangeSchemeCommand::undo()
{
	QMapIterator<int, DocumentFloss *> flossIterator(m_flosses);
	while (flossIterator.hasNext())
	{
		int key = flossIterator.key();
		m_flosses[key] = m_document->documentPalette().replace(key, m_flosses[key]);
	}

	QString schemeName = m_document->documentPalette().schemeName();
	m_document->documentPalette().setSchemeName(m_schemeName);
	m_schemeName = schemeName;
}


EditorReadDocumentSettingsCommand::EditorReadDocumentSettingsCommand(Editor *editor)
	:	QUndoCommand(),
		m_editor(editor)
{
}


EditorReadDocumentSettingsCommand::~EditorReadDocumentSettingsCommand()
{
}

void EditorReadDocumentSettingsCommand::redo()
{
	m_editor->readDocumentSettings();
}


void EditorReadDocumentSettingsCommand::undo()
{
	m_editor->readDocumentSettings();
}


PaletteReadDocumentSettingsCommand::PaletteReadDocumentSettingsCommand(Palette *palette)
	:	QUndoCommand(),
		m_palette(palette)
{
}


PaletteReadDocumentSettingsCommand::~PaletteReadDocumentSettingsCommand()
{
}


void PaletteReadDocumentSettingsCommand::redo()
{
	m_palette->readDocumentSettings();
}


void PaletteReadDocumentSettingsCommand::undo()
{
	m_palette->readDocumentSettings();
}


PreviewReadDocumentSettingsCommand::PreviewReadDocumentSettingsCommand(Preview *preview)
	:	QUndoCommand(),
		m_preview(preview)
{
}


PreviewReadDocumentSettingsCommand::~PreviewReadDocumentSettingsCommand()
{
}


void PreviewReadDocumentSettingsCommand::redo()
{
	m_preview->readDocumentSettings();
}


void PreviewReadDocumentSettingsCommand::undo()
{
	m_preview->readDocumentSettings();
}


PaletteReplaceColorCommand::PaletteReplaceColorCommand(Document *document, int originalIndex, int replacementIndex)
	:	QUndoCommand(i18n("Replace Color")),
		m_document(document),
		m_originalIndex(originalIndex),
		m_replacementIndex(replacementIndex)
{
}


PaletteReplaceColorCommand::~PaletteReplaceColorCommand()
{
}


void PaletteReplaceColorCommand::redo()
{
	if (m_stitches.count() || m_backstitches.count() || m_knots.count())
	{
		// populated from a previous redo call
		// iterator over the existing pointers
		QListIterator<Stitch *> stitchIterator(m_stitches);
		while (stitchIterator.hasNext())
		{
			stitchIterator.next()->colorIndex = m_replacementIndex;
		}

		QListIterator<Backstitch *> backstitchIterator(m_backstitches);
		while (backstitchIterator.hasNext())
		{
			backstitchIterator.next()->colorIndex = m_replacementIndex;
		}

		QListIterator<Knot *> knotIterator(m_knots);
		while (knotIterator.hasNext())
		{
			knotIterator.next()->colorIndex = m_replacementIndex;
		}
	}
	else
	{
		// search the stitch data for stitches of the required color
		StitchData &stitchData = m_document->stitchData();
		QListIterator<int> stitchLayerIterator(stitchData.stitchLayers());
		while (stitchLayerIterator.hasNext())
		{
			int layer = stitchLayerIterator.next();
			for (int row = 0 ; row < stitchData.height() ; ++row)
			{
				for (int col = 0 ; col < stitchData.width() ; ++col)
				{
					StitchQueue *queue = stitchData.stitchQueueAt(layer, QPoint(col, row));
					if (queue)
					{
						QListIterator<Stitch *> stitchIterator(*queue);
						while (stitchIterator.hasNext())
						{
							Stitch *stitch = stitchIterator.next();
							if (stitch->colorIndex == m_originalIndex)
							{
								m_stitches.append(stitch);
								stitch->colorIndex = m_replacementIndex;
							}
						}
					}
				}
			}
		}

		QListIterator<int> backstitchLayerIterator(stitchData.backstitchLayers());
		while (backstitchLayerIterator.hasNext())
		{
			int layer = backstitchLayerIterator.next();
			QListIterator<Backstitch *> backstitchIterator = stitchData.backstitchIterator(layer);
			while (backstitchIterator.hasNext())
			{
				Backstitch *backstitch = backstitchIterator.next();
				if (backstitch->colorIndex == m_originalIndex)
				{
					m_backstitches.append(backstitch);
					backstitch->colorIndex = m_replacementIndex;
				}
			}
		}

		QListIterator<int> knotLayerIterator(stitchData.knotLayers());
		while (knotLayerIterator.hasNext())
		{
			int layer = knotLayerIterator.next();
			QListIterator<Knot *> knotIterator = stitchData.knotIterator(layer);
			while (knotIterator.hasNext())
			{
				Knot *knot = knotIterator.next();
				if (knot->colorIndex == m_originalIndex)
				{
					m_knots.append(knot);
					knot->colorIndex = m_replacementIndex;
				}
			}
		}
	}
}


void PaletteReplaceColorCommand::undo()
{
	QListIterator<Stitch *> stitchIterator(m_stitches);
	while (stitchIterator.hasNext())
	{
		stitchIterator.next()->colorIndex = m_originalIndex;
	}

	QListIterator<Backstitch *> backstitchIterator(m_backstitches);
	while (backstitchIterator.hasNext())
	{
		backstitchIterator.next()->colorIndex = m_originalIndex;
	}

	QListIterator<Knot *> knotIterator(m_knots);
	while (knotIterator.hasNext())
	{
		knotIterator.next()->colorIndex = m_originalIndex;
	}
}


PaletteSwapColorCommand::PaletteSwapColorCommand(Document *document, int originalIndex, int swappedIndex)
	:	QUndoCommand(i18n("Swap Colors")),
		m_document(document),
		m_originalIndex(originalIndex),
		m_swappedIndex(swappedIndex)
{
}


PaletteSwapColorCommand::~PaletteSwapColorCommand()
{
}


void PaletteSwapColorCommand::redo()
{
	m_document->documentPalette().swap(m_originalIndex, m_swappedIndex);
}


void PaletteSwapColorCommand::undo()
{
	m_document->documentPalette().swap(m_originalIndex, m_swappedIndex);
}


UpdatePrinterConfigurationCommand::UpdatePrinterConfigurationCommand(Document *document, const PrinterConfiguration &printerConfiguration)
	:	m_document(document),
		m_printerConfiguration(printerConfiguration)
{
}


UpdatePrinterConfigurationCommand::~UpdatePrinterConfigurationCommand()
{
}


void UpdatePrinterConfigurationCommand::redo()
{
	PrinterConfiguration original = m_document->printerConfiguration();
	m_document->setPrinterConfiguration(m_printerConfiguration);
	m_printerConfiguration = original;
}


void UpdatePrinterConfigurationCommand::undo()
{
	redo();
}
