
#define AcChild TGT_AC_CHILD
#define AcEnd TGT_AC_END,0
#define AcHorizGroup TGT_AC_HORIZONTALGROUP
#define AcVertGroup TGT_AC_VERTICALGROUP
#define AcSpace(x) TGT_AC_SPACE, x
#define AcSetSpacing(x) TGT_AC_SPACING, x
#define AcMinWidth(x) TGT_AC_XS, x
#define AcMinHeight(x) TGT_AC_YS, x
#define AcAlign(x) TGT_AC_ALIGNMENT, x
#define AcSaveAs(x) TGT_AC_SAVEAS, &(x)
#define End TGTT_END, 0




#define Id(x) TGTT_ID, (tgtt) (x)
#define Callback(x) TGTT_CALLBACK, (tgtt) (x)
#define Width(x) TGTT_XS, (x)
#define Height(x) TGTT_YS, (x)
#define Color(x) TGTT_FG, (x)
#define Background(x) TGTT_BG, (x)

#define MinValue(x) TGTT_GENERIC_MIN, (x)
#define MaxValue(x) TGTT_GENERIC_MAX, (x)
#define Value(x) TGTT_GENERIC_VALUE, (x)
#define SelColor(x) TGTT_GENERIC_ACTIVEBG, (x)
#define Caption(x) TGTT_GENERIC_CAPTION, (tgtt) (x)
#define Text(x) Caption(x)
#define FrameColor(x) TGTT_GENERIC_FRAMECOLOR, (x)
#define BorderColor(x) FrameColor(x)




#define Button TGT_CLASS_BUTTON
#define Checkbox TGT_CLASS_CHECKBOX
#define Direct TGT_CLASS_DIRECT
#define Label TGT_CLASS_LABEL
#define List TGT_CLASS_LIST
#define Progress TGT_CLASS_PROGRESS
#define Selectbox TGT_CLASS_SELECTBOX
#define Slider TGT_CLASS_SLIDER
#define String TGT_CLASS_STRING
#define Termemu TGT_CLASS_TERMEMU
#define TextBuffer TGT_CLASS_BUFFER

#define CheckboxState(x) TGTT_CHECKBOX_ACTIVE, (x)

#define LabelType(x) TGTT_LABEL_TYPE, (x)

#define ListItems(x) TGTT_LIST_ITEMS, (tgtt) (x)
#define ListSelFGColor(x) TGTT_LIST_ACTIVEFG, (x)
#define ListDataC(x) TGTT_LIST_DATACALLBACK, (tgtt) (x)
#define ListChangeC(x) TGTT_LIST_CHANGECALLBACK, (tgtt) (x)

#define ProgressType(x) TGTT_PROGRESS_TYPE, x

#define SelectboxItems(x) TGTT_SELECTBOX_ITEMS, (tgtt) (x)
#define SelectboxMarks(a,b) TGTT_SELECTBOX_MARK1, (tgtt) (a), TGTT_SELECTBOX_MARK2, (tgtt) (b)
#define SelectboxSelected(x) TGTT_SELECTBOX_SELECTED, x

#define SliderType(x) TGTT_SLIDER_TYPE, x

#define StringString(x) TGTT_STRING_STRING, (tgtt) (x)
#define StringHash(x) TGTT_STRING_HASHED, (tgtt) (x)
#define StringAccept(x) TGTT_STRING_ACCEPT, (tgtt) (x)
#define StringEmpty(x) TGTT_STRING_EMPTYCHAR, (tgtt) (x)

#define BufferSize(x) TGTT_BUFFER_BUFFERSIZE, x

#define SimpleButton(x) Button, Caption(x), End
#define SimpleButtonI(x, i) Button, Caption(x), Id(i), End
#define SimpleButtonC(x, c) Button, Caption(x), Callback(c), End
#define SimpleButtonIC(x, i, c) Button, Caption(x), Id(i), Callback(c), End
#define SimpleButtonCI(x, c, i) SimpleButtonIC(x, i, c)

#define SimpleCheckbox(x, v) Checkbox, Caption(x), CheckboxState(v), End
#define SimpleCheckboxI(x, v, i) Checkbox, Caption(x), CheckboxState(v), Id(i), End
#define SimpleCheckboxC(x, v, c) Checkbox, Caption(x), CheckboxState(v), Callback(c), End
#define SimpleCheckboxIC(x, v, i, c) Checkbox, Caption(x), CheckboxState(v), Id(i), Callback(c), End
#define SimpleCheckboxCI(x, v, c, i) SimpleCheckboxIC(x, i, c)

#define SimpleDirect(x, y) Direct, Width(x), Height(y), End
#define SimpleDirectI(x, y, id) Direct, Width(x), Height(y), Id(id), End

#define SimpleLabel(x) Label, Text(x), End
#define SimpleLabelI(x, id) Label, Text(x), Id(id), End

#define SimpleList(x) List, ListItems(x), End
#define SimpleListI(x, i) List, ListItems(x), Id(i), End
#define SimpleListC(x, c) List, ListItems(x), Callback(c), End
#define SimpleListIC(x, i, c) List, ListItems(x), Id(i), Callback(c), End
#define SimpleListCI(x, i, c) SimpleListIC(x,i,c)

#define SimpleProgress(x, c, m) Progress, Caption(x), Value(c), MaxValue(m), End
#define SimpleProgressI(x, c, m, id) Progress, Caption(x), Value(c), MaxValue(m), Id(i), End

#define SimpleSelectbox(x) Selectbox, SelectboxItems(x), End
#define SimpleSelectboxI(x,i) Selectbox, SelectboxItems(x), Id(i), End
#define SimpleSelectboxC(x,c) Selectbox, SelectboxItems(x), Callback(c), End
#define SimpleSelectboxIC(x,i,c) Selectbox, SelectboxItems(x), Callback(c), Id(i), End
#define SimpleSelectboxCI(x,c,i) SimpleSelectboxIC(x,i,c)

#define SimpleSlider(x, min, curr, max) \
		Slider, Caption(x), MinValue(min), Value(curr), MaxValue(max), End	
#define SimpleSliderI(x, min, curr, max,id) \
		Slider, Caption(x), MinValue(min), Value(curr), MaxValue(max), Id(id), End
#define SimpleSliderC(x, min, curr, max,c) \
		Slider, Caption(x), MinValue(min), Value(curr), MaxValue(max), Callback(c), End
#define SimpleSliderIC(x, min, curr, max,i,c) \
		Slider, Caption(x), MinValue(min), Value(curr), MaxValue(max), Callback(c), Id(i), End
#define SimpleSliderCI(x, min, curr, max, c, i) SimpleSliderIC(x, min, curr, max, i, c)

#define SimpleString(max) String, MaxValue(max), End
#define SimpleStringI(max,i) String, MaxValue(max), Id(i), End
#define SimpleStringC(max,c) String, MaxValue(max), Callback(c), End
#define SimpleStringIC(max,i,c) String, MaxValue(max), Id(i), Callback(c), End
#define SimpleStringCI(max,c,i) SimpleStringIC(max,i,c)


#define PasswordString(max) String, MaxValue(max), StringHash('*'), End
#define PasswordStringI(max,i) String, MaxValue(max), StringHash('*'), Id(i), End
#define PasswordStringC(max,c) String, MaxValue(max), StringHash('*'), Callback(c), End
#define PasswordStringIC(max,i,c) String, MaxValue(max), StringHash('*'), Id(i), Callback(c), End
#define PasswordStringCI(max,c,i) PasswordStringIC(max,i,c)

#define TermemuI(i) Termemu, Id(i), End
#define TermemuC(c) Termemu, Callback(c), End
#define TermemuIC(i,c) Termemu, Id(i), Callback(c), End
#define TermemuCI(c,i) TermemuIC(i,c)

#define BufferI(i) TextBuffer, Id(i), End
#define BufferC(c) TextBuffer, Callback(c), End
#define BufferIC(i,c) TextBuffer, Id(i), Callback(c), End
#define BufferCI(c,i) BufferIC(i,c)
