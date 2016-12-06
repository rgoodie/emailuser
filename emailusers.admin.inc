<?php

/**
 * @file
 * TODO: Enter file description here.
 */

/**
 * Form builder.
 */
function emailusers_settings_form($form, &$form_state) {
  $form['emailusers_default_subj_line'] = array(
    '#type' => 'textfield',
    '#title' => t('Default subject line'),
    '#default_value' => t(variable_get('emailusers_default_subj_line', 'Email from this site. ' . variable_get('site_name', ''))),
    '#required' => TRUE,
  );

  return system_settings_form($form);
}
