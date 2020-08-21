/* Any copyright is dedicated to the Public Domain.
 http://creativecommons.org/publicdomain/zero/1.0/ */

"use strict";

// Tests that the flexbox highlighter is hidden when the highlighted flexbox container is
// removed from the page.

const TEST_URI = `
  <style type='text/css'>
    #flex {
      display: flex;
    }
  </style>
  <div id="flex"></div>
`;

add_task(async function() {
  await addTab("data:text/html;charset=utf-8," + encodeURIComponent(TEST_URI));
  const { inspector, view, testActor } = await openRuleView();
  const { highlighters } = view;

  const onRuleViewUpdated = view.once("ruleview-refreshed");
  await selectNode("#flex", inspector);
  await onRuleViewUpdated;
  const container = getRuleViewProperty(view, "#flex", "display").valueSpan;
  const flexboxToggle = container.querySelector(".ruleview-flex");

  info("Toggling ON the flexbox highlighter from the rule-view.");
  const onHighlighterShown = highlighters.once("flexbox-highlighter-shown");
  flexboxToggle.click();
  await onHighlighterShown;
  ok(highlighters.flexboxHighlighterShown, "Flexbox highlighter is shown.");

  info("Remove the #flex container in the content page.");
  const onHighlighterHidden = highlighters.once("flexbox-highlighter-hidden");
  testActor.eval(`document.querySelector("#flex").remove();`);
  await onHighlighterHidden;
  ok(!highlighters.flexboxHighlighterShown, "Flexbox highlighter is hidden.");
});