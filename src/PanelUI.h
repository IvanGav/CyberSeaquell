#pragma once

#include "UI.h"
#include "terminal/terminal.h"
#include "CyberSeaquell_decl.h"

namespace PanelUI {
struct Panel;
Panel* alloc_panel();
void free_panel(Panel* panel);

Panel* rootPanel;

const F32 PANEL_MIN_SIZE = 16.0F;
struct Panel {
	Panel* parent;
	Panel* childA;
	Panel* childB;

	UI::BoxHandle uiBox;
	UI::BoxHandle content;

	void build_ui() {
		using namespace UI;
		Box* panelBox = uiBox.get();
		if (!panelBox) {
			return;
		}

		BoxHandle prevWorkingBox = workingBox;
		workingBox = BoxHandle{ panelBox, panelBox->generation };

		content = generic_box();
		Box* contentBox = content.unsafeBox;
		contentBox->idealSize = V2F32{ BOX_INF_SIZE, BOX_INF_SIZE };
		contentBox->backgroundColor = V4F32{ 0.1F, 0.1F, 0.1F, 1.0F }.to_rgba8();
		contentBox->flags = BOX_FLAG_CLIP_CHILDREN | BOX_FLAG_CUSTOM_DRAW | BOX_FLAG_DONT_LAYOUT_TO_FIT_CHILDREN;
		contentBox->hoverCursor = Win32::CURSOR_TYPE_SIZE_ALL;
		contentBox->actionCallback = [](UI::Box* box, UI::UserCommunication& comm) {
			Panel& panel = *reinterpret_cast<Panel*>(box->userData[0]);
			UI::ActionResult result = UI::ACTION_PASS;
			if (comm.tessellator) {
				result = UI::ACTION_HANDLED;
			}
			if (comm.scrollInput != 0.0F) {
				V2F32 scaleCenter = comm.mousePos - box->computedOffset - box->contentOffset;
				F32 scaleFactor = 1.0F + clamp(comm.scrollInput * 0.001F, -0.2F, 0.2F);
				F32 newScale = clamp(box->contentScale * scaleFactor, 0.2F, 5.0F);
				F32 scaleFactorAfterClamp = newScale / box->contentScale;
				box->contentOffset += scaleCenter - scaleCenter * scaleFactorAfterClamp;
				box->contentScale = newScale;
				result = UI::ACTION_HANDLED;
			}
			if ((comm.drag.x != 0.0F || comm.drag.y != 0.0F) && box == UI::activeBox.get()) {
				box->contentOffset += comm.drag;
				result = UI::ACTION_HANDLED;
			}
			V2F32 mouseRelative = (comm.mousePos - box->computedOffset - box->contentOffset) / box->contentScale;
			return result;
		};
		content.unsafeBox->userData[0] = UPtr(this);

		workingBox = prevWorkingBox;
	}

	B32 split(Axis2 axis) {
		Panel* newParent = alloc_panel();
		Panel* a = this;
		Panel* b = alloc_panel();
		newParent->uiBox = UI::alloc_box();
		newParent->uiBox.unsafeBox->flags = UI::BOX_FLAG_DONT_LAYOUT_TO_FIT_CHILDREN;
		newParent->uiBox.unsafeBox->sizeParentPercent = a->uiBox.unsafeBox->sizeParentPercent;
		b->uiBox = UI::alloc_box();
		b->uiBox.unsafeBox->flags = UI::BOX_FLAG_DONT_LAYOUT_TO_FIT_CHILDREN;
		b->uiBox.unsafeBox->sizeParentPercent = V2F32{ 1.0F, 1.0F };
		a->uiBox.unsafeBox->sizeParentPercent = V2F32{ 1.0F, 1.0F };

		DLL_REPLACE(uiBox.unsafeBox, newParent->uiBox.unsafeBox, uiBox.unsafeBox->parent->childFirst, uiBox.unsafeBox->parent->childLast, prev, next);

		if (parent) {
			*parent->child_ref(this) = newParent;
		} else {
			rootPanel = newParent;
		}

		newParent->uiBox.unsafeBox->parent = uiBox.unsafeBox->parent;
		newParent->parent = parent;
		a->parent = newParent;
		b->parent = newParent;
		newParent->childA = a;
		newParent->childB = b;

		a->uiBox.unsafeBox->parent = newParent->uiBox.unsafeBox;
		b->uiBox.unsafeBox->parent = newParent->uiBox.unsafeBox;

		b->build_ui();

		UI::BoxHandle draggableCenter = UI::alloc_box();
		draggableCenter.unsafeBox->parent = newParent->uiBox.unsafeBox;
		draggableCenter.unsafeBox->minSize.v[axis] = 3.0F;
		draggableCenter.unsafeBox->sizeParentPercent.v[axis2_orthogonal(axis)] = 1.0F;
		draggableCenter.unsafeBox->hoverCursor = axis == AXIS2_X ? Win32::CURSOR_TYPE_SIZE_HORIZONTAL : Win32::CURSOR_TYPE_SIZE_VERTICAL;
		draggableCenter.unsafeBox->backgroundColor = V4F32{ 0.7F, 0.7F, 0.7F, 1.0F }.to_rgba8();
		draggableCenter.unsafeBox->userData[0] = axis;
		draggableCenter.unsafeBox->actionCallback = [](UI::Box* box, UI::UserCommunication& com) {
			Axis2 splitAxis = Axis2(box->userData[0]);
			if (com.drag.v[splitAxis]) {
				F32 percentA = box->prev->sizeParentPercent.v[splitAxis];
				F32 percentB = box->next->sizeParentPercent.v[splitAxis];
				F32 normalizedDistance = percentA / (percentA + percentB);
				F32 parentRange = box->parent->computedSize.v[splitAxis];
				F32 currentSplitPos = parentRange * normalizedDistance;
				F32 nextSplitPos = clamp(currentSplitPos + com.drag.v[splitAxis], 0.0F, parentRange);
				box->prev->sizeParentPercent.v[splitAxis] = nextSplitPos / parentRange;
				box->next->sizeParentPercent.v[splitAxis] = 1.0F - box->prev->sizeParentPercent.v[splitAxis];
				return UI::ACTION_HANDLED;
			}
			return UI::ACTION_PASS;
		};
		UI::Box* parentBox = newParent->uiBox.unsafeBox;
		parentBox->layoutDirection = axis == AXIS2_X ? UI::LAYOUT_DIRECTION_RIGHT : UI::LAYOUT_DIRECTION_DOWN;
		DLL_INSERT_TAIL(a->uiBox.unsafeBox, parentBox->childFirst, parentBox->childLast, prev, next);
		DLL_INSERT_TAIL(draggableCenter.unsafeBox, parentBox->childFirst, parentBox->childLast, prev, next);
		DLL_INSERT_TAIL(b->uiBox.unsafeBox, parentBox->childFirst, parentBox->childLast, prev, next);
		return true;
	}

	void destroy() {
		if (!parent || childA || childB) {
			return;
		}
		UI::Box* parentBox = parent->uiBox.unsafeBox;
		UI::free_box(UI::BoxHandle{ parentBox->childFirst->next, parentBox->childFirst->next->generation });
		UI::free_box(uiBox);
		Panel* sibling = *parent->sibling_ref(this);
		UI::Box* siblingBox = sibling->uiBox.unsafeBox;
		siblingBox->sizeParentPercent = parentBox->sizeParentPercent;
		DLL_REMOVE(siblingBox, parentBox->childFirst, parentBox->childLast, prev, next);
		DLL_REPLACE(parentBox, siblingBox, parentBox->parent->childFirst, parentBox->parent->childLast, prev, next);
		siblingBox->parent = parentBox->parent;
		UI::free_box(parent->uiBox);
		if (parent->parent) {
			*parent->parent->child_ref(parent) = sibling;
		} else {
			rootPanel = sibling;
		}
		sibling->parent = parent->parent;
		free_panel(parent);
		free_panel(this);
	}

	Panel** sibling_ref(Panel* child) {
		return child == childA ? &childB :
			child == childB ? &childA :
			nullptr;
	}
	Panel** child_ref(Panel* child) {
		return child == childA ? &childA :
			child == childB ? &childB :
			nullptr;
	}
};

Panel* panelFreeList;

Panel* alloc_panel() {
	if (!panelFreeList) {
		panelFreeList = globalArena.alloc<Panel>(1);
		panelFreeList->childA = nullptr;
	}
	Panel* panel = panelFreeList;
	panelFreeList = panel->childA;
	*panel = Panel{};
	return panel;
}

void free_panel(Panel* panel) {
	panel->childA = panelFreeList;
	panelFreeList = panel;
}

UI::Box* termBox;
UI::Box* cams[8];
UI::Box* terminals[4];
B32 terminalActive;
const F32 terminalTextHeight = 20.0F;

void init() {
	using namespace UI;
	Panel* panel = alloc_panel();
	panel->uiBox = UI::alloc_box();
	panel->uiBox.unsafeBox->flags = UI::BOX_FLAG_DONT_LAYOUT_TO_FIT_CHILDREN;
	panel->uiBox.unsafeBox->sizeParentPercent = V2F32{ 1.0F, 1.0F };
	rootPanel = panel;
	panel->build_ui();
	panel->uiBox.unsafeBox->parent = UI::root.unsafeBox;
	DLL_INSERT_HEAD(panel->uiBox.unsafeBox, UI::root.unsafeBox->childFirst, UI::root.unsafeBox->childLast, prev, next);

	panel->split(AXIS2_X);
	panel = panel->parent;
	UI_WORKING_BOX(panel->childA->content) {
		UI_FLAGS(defaultFlagsStack.back() | BOX_FLAG_FLOATING_X | BOX_FLAG_FLOATING_Y) {
			UI_FLAGS(defaultFlagsStack.back() | BOX_FLAG_DISABLED)
			UI_SIZE((V2F32{ 32.0F, 32.0F }))
			UI_BACKGROUND_COLOR((V4F32{ 1.0F, 1.0F, 1.0F, 1.0F })) {
				(cams[0] = button(Textures::camRed, [](Box* b) { b->backgroundTexture = &Textures::camBee; }).unsafeBox)->contentOffset = V2F32{ 1500.0F, 1000.0F } *0.5F;
				(cams[1] = button(Textures::camRed, [](Box* b) { b->backgroundTexture = &Textures::camBee; }).unsafeBox)->contentOffset = V2F32{ 1500.0F, 762.0F } *0.5F;
				(cams[2] = button(Textures::camRed, [](Box* b) { b->backgroundTexture = &Textures::camBee; }).unsafeBox)->contentOffset = V2F32{ 1108.0F, 542.0F } *0.5F;
				(cams[3] = button(Textures::camRed, [](Box* b) { b->backgroundTexture = &Textures::camBee; }).unsafeBox)->contentOffset = V2F32{ 1104.0F, 295.0F } *0.5F;
				(cams[4] = button(Textures::camRed, [](Box* b) { b->backgroundTexture = &Textures::camBee; }).unsafeBox)->contentOffset = V2F32{ 1510.0F, 160.0F } *0.5F;
				(cams[5] = button(Textures::camRed, [](Box* b) { b->backgroundTexture = &Textures::camBee; }).unsafeBox)->contentOffset = V2F32{ 1050.0F, 240.0F } *0.5F;
				(cams[6] = button(Textures::camRed, [](Box* b) { b->backgroundTexture = &Textures::camBee; }).unsafeBox)->contentOffset = V2F32{ 655.0F, 33.0F } *0.5F;
				(cams[7] = button(Textures::camRed, [](Box* b) { b->backgroundTexture = &Textures::camBee; }).unsafeBox)->contentOffset = V2F32{ 758.0F, 379.0F } *0.5F;


				(terminals[0] = button(Textures::terminal, [](Box* b) {  }).unsafeBox)->contentOffset = V2F32{ 1107.0F, 152.0F } *0.5F;
				(terminals[1] = button(Textures::terminal, [](Box* b) {  }).unsafeBox)->contentOffset = V2F32{ 856.0F, 254.0F } *0.5F;
				(terminals[2] = button(Textures::terminal, [](Box* b) {  }).unsafeBox)->contentOffset = V2F32{ 798.0F, 588.0F } *0.5F;
				(terminals[3] = button(Textures::terminal, [](Box* b) {  }).unsafeBox)->contentOffset = V2F32{ 749.0F, 630.0F } *0.5F;
			}
			
			UI_SIZE((V2F32{ 1920.0F, 1080.0F } *0.5F))
			UI_BACKGROUND_COLOR((V4F32{ 1.0F, 1.0F, 1.0F, 1.0F }))
			generic_box().unsafeBox->backgroundTexture = &Textures::map[7];
		}
	}
	cams[0]->flags &= ~BOX_FLAG_DISABLED;
	cams[0]->backgroundTexture = &Textures::camBee;
	terminals_init();
	open_terminal(0);
	terminalActive = true;
	termBox = panel->childB->content.unsafeBox;
	panel->childB->content.unsafeBox->actionCallback = [](Box* box, UserCommunication& comm) {
		Panel& panel = *reinterpret_cast<Panel*>(box->userData[0]);
		if (comm.tessellator) {
			comm.tessellator->ui_rect2d(comm.renderArea.minX, comm.renderArea.minY, comm.renderArea.maxX, comm.renderArea.maxY, comm.renderZ, 0.0F, 0.0F, 1.0F, 1.0F, V4F32{ 0.0F, 0.0F, 0.0F, 1.0F }, Textures::simpleWhite.index, comm.clipBoxIndex << 16);
			if (terminalActive) {
				file& file = get_terminal();
				I32 heightInChars = I32((comm.renderArea.maxY - comm.renderArea.minY) / terminalTextHeight);
				I32 offset = get_offset(heightInChars);
				for (U32 i = offset; i < min(offset + heightInChars, I32(file.size())); i++) {
					TextRenderer::draw_string_batched(*comm.tessellator, StrA{ file[i].c_str(), file[i].length() }, comm.renderArea.minX, comm.renderArea.minY + F32(i - offset) * terminalTextHeight, comm.renderZ, terminalTextHeight, V4F32{ 0.7F, 0.7F, 0.7F, 1.0F }, comm.clipBoxIndex << 16);
				}
				if (U64(CyberSeaquell::totalTime * 2.0) & 1) {
					F32 cursorX = comm.renderArea.minX + F32(cursor_x()) * TextRenderer::string_size_x("a"sa, terminalTextHeight);
					F32 cursorY = comm.renderArea.minY + F32(cursor_y() - offset) * terminalTextHeight;
					comm.tessellator->ui_rect2d(cursorX, cursorY, cursorX + 2.0F, cursorY + terminalTextHeight, comm.renderZ, 0.0F, 0.0F, 1.0F, 1.0F, V4F32{ 1.0F, 1.0F, 1.0F, 1.0F }, Textures::simpleWhite.index, comm.clipBoxIndex << 16);
				}
			}
			return ACTION_HANDLED;
		}
		if (terminalActive && comm.keyPressed) {
			if (type_char(comm.keyPressed, comm.charTyped)) {
				terminalActive = false;
			}
			return ACTION_HANDLED;
		}
		if (terminalActive && comm.scrollInput) {
			scroll_input(comm.scrollInput);
			return ACTION_HANDLED;
		}
		if (terminalActive && comm.leftClicked) {
			V2F32 mouseRelative = (comm.mousePos - box->computedOffset - box->contentOffset) / box->contentScale;
			click_at(I32(mouseRelative.x / TextRenderer::string_size_x("a"sa, terminalTextHeight) + 0.5F), I32(mouseRelative.y / terminalTextHeight));
			return ACTION_HANDLED;
		}
		return ACTION_PASS;
	};
	panel->childB->content.unsafeBox->hoverCursor = Win32::CURSOR_TYPE_POINTER;
}
}