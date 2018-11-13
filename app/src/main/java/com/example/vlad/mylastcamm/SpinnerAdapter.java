package com.example.vlad.mylastcamm;

import android.content.Context;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import java.util.ArrayList;

public class SpinnerAdapter extends ArrayAdapter<SpinnerItem> {
    public SpinnerAdapter(Context context, ArrayList<SpinnerItem> metricsList){
        super(context,0, metricsList);
    }

    @NonNull
    @Override
    public View getView(int position, @Nullable View convertView, @NonNull ViewGroup parent) {
        return initView(position, convertView, parent);
    }

    @Override
    public View getDropDownView(int position,@Nullable View convertView, @NonNull ViewGroup parent) {
        return initView(position, convertView, parent);
    }

    private View initView(int position, View convertView, ViewGroup parent){
        if (convertView == null){
            convertView = LayoutInflater.from(getContext()).inflate(
                    R.layout.style_spinner, parent, false);


        }
        TextView textViewName = convertView.findViewById(R.id.text_view_name);

        SpinnerItem currentItem = getItem(position);

        if (currentItem != null) {
            textViewName.setText(currentItem.getmMetricsName());
        }
        return convertView;
    }
}
